#
# Copyright (C) 2024  SUSE Software Solutions Germany GmbH
#
# This file is part of klp-ccp.
#
# klp-ccp is free software: you can redistribute it and/or modify it
# under the terms of version 2 of the GNU General Public License as
# published by the Free Software Foundation.
#
# klp-ccp is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with klp-ccp. If not, see <https://www.gnu.org/licenses/>.
#

from dataclasses import dataclass
from enum import IntEnum
from functools import total_ordering

@total_ordering
@dataclass
class NodeId:
    name: str
    ipa_id: int
    filename: str
    line: int
    column: int

    def __lt__(self, other):
        if self.ipa_id != other.ipa_id:
            return self.ipa_id < other.ipa_id
        else:
            return self.name < other.name

    def __eq__(self, other):
        return self.ipa_id == other.ipa_id and self.name == other.name

    def __hash__(self):
        return hash(self.ipa_id)

class Node:
    def __init__(self, node_id):
        self.node_id = node_id
        self.removed = False
        self._in_edges = []
        self._out_edges = []

    def is_ipa_opt_clone(self):
        return any(edge.is_ipa_opt_clone() for edge in self._in_edges)

class EdgeType(IntEnum):
    INLINED = 0
    ISRA = 1
    CONSTPROP = 2
    PART = 3
    DUPLICATE_OPT_CLONE = 4

@total_ordering
class Edge:
    def __init__(self, original, spawn, edge_type):
        self.original = original
        self.spawn = spawn
        self.edge_type = edge_type

    def is_inline(self):
        return self.edge_type == EdgeType.INLINED

    def is_ipa_opt_clone(self):
        return not self.is_inline()

    def __lt__(self, other):
        if self.original != other.original:
            return self.original < other.original
        elif self.spawn != other.spawn:
            return self.spawn < other.spawn
        else:
            return self.edge_type < other.edge_type

    def __eq__(self, other):
        return (self.original == other.original and
                self.spawn == other.spawn and
                self.edge_type == other.edge_type)

    def __hash__(self):
        return hash((self.original, self.spawn, self.edge_type))

    def _link_to_nodes(self):
        self.original._out_edges.append(self)
        self.spawn._in_edges.append(self)

class IpaClones:
    def __init__(self, filename):
        self._nodes = []
        self._nodes_by_name = {}

        nodes = dict()
        def add_node(node):
            if not node.node_id in nodes:
                nodes[node.node_id] = node
                self._nodes.append(node)
                if not node.node_id.name in self._nodes_by_name:
                    self._nodes_by_name[node.node_id.name] = [node]
                else:
                    self._nodes_by_name[node.node_id.name].append(node)
                return node
            else:
                return nodes[node.node_id]

        edges = set()

        f = open(filename)
        for line in f.readlines():
            line = line.strip();
            line = line.split(';')
            if len(line) == 0:
                raise ValueError('Unrecognized input line in IPA clones \"' +
                                 filename + '\"')
            if line[0] == 'Callgraph clone':
                if (len(line) == 14):
                    # Old format
                    if line[6] != '<-' or line[-2] != 'optimization:':
                        raise (ValueError
                               ('Unrecognized input line in IPA clones \"' +
                                filename + '\"'))
                    original = NodeId(*line[1:6])
                    spawn = NodeId(*line[7:12])
                elif len(line) == 12:
                    # New format
                    original = NodeId(*line[1:6])
                    spawn = NodeId(*line[6:11])
                else:
                    raise (ValueError
                           ('Unrecognized input line in IPA clones \"' +
                            filename + "\""))

                match line[-1]:
                    case 'inlining to':
                        edge_type = EdgeType.INLINED
                    case 'isra':
                        edge_type = EdgeType.ISRA
                    case 'constprop':
                        edge_type = EdgeType.CONSTPROP
                    case 'part':
                        edge_type = EdgeType.PART
                    case _:
                        raise (ValueError
                               ('Unrecognized optimization \"' + line[-1] +
                                '\" in IPA clones \"' + filename + '\"'))

                original = add_node(Node(original))
                spawn = add_node(Node(spawn))

                edge = Edge(original, spawn, edge_type)
                if edge in edges:
                    next
                edges.add(edge)
                edge._link_to_nodes()

            elif line[0] == 'Callgraph removal':
                if len(line) != 6:
                    raise (ValueError
                           ('Unrecognized input line in IPA clones \"' +
                            filename + "\""))
                node = NodeId(*line[1:6])
                node = add_node(Node(node))
                node.removed = True

            else:
                raise ValueError('Unrecognized input line in IPA clones \"' +
                                 filename + "\"")
        f.close()

        # IPA opt clones can apparently emerge out of thin air by
        # duplicating another with an operation recorded as inlining.
        # These duplicates would have the very same name, but a
        # different IPA ID assigned to them. Rewrite the edges to
        # restore the fact that such an instance is in fact an
        # optimization clone (rather than originating from mere
        # inlining).
        worklist = list(filter(lambda node: node.is_ipa_opt_clone(),
                               self._nodes))
        while worklist:
            node = worklist.pop()
            for edge in node._out_edges:
                if (not edge.is_inline() or
                    node.node_id.name != edge.spawn.node_id.name):
                    continue
                spawn_was_ipa_opt_clone = edge.spawn.is_ipa_opt_clone()
                edge.edge_type = EdgeType.DUPLICATE_OPT_CLONE
                if (not spawn_was_ipa_opt_clone and
                    edge.spawn.is_ipa_opt_clone()):
                    worklist.append(edge.spawn)

        self._find_strongly_connected_components()

    def _find_strongly_connected_components(self):
        # Strongly connected component decomposition, c.f. "Depth-First
        # Search and Linear Graph Algorithms", R. Tarjan,
        # https://doi.org/10.1137/0201010
        class NodeSccAux:
            def __init__(self):
                self.dfs_pre_num = None
                self.lowlink = None
                self.on_scc_stack = False

        for node in self._nodes:
            node.scc_aux = NodeSccAux()

        next_dfs_pre_num = 0
        sc_components = []
        for v in self._nodes:
            if v.scc_aux.dfs_pre_num is not None:
                continue

            v.scc_aux.dfs_pre_num = next_dfs_pre_num
            v.scc_aux.lowlink = next_dfs_pre_num
            next_dfs_pre_num += 1
            scc_stack = [v]
            v.scc_aux.on_scc_stack = True
            v_edges_iter = iter(v._out_edges)
            walk_stack = []
            while v is not None:
                edge = next(v_edges_iter, None)
                if edge is not None:
                    assert(v == edge.original)
                    w = edge.spawn
                    if w.scc_aux.dfs_pre_num is None:
                        # Node not numbered yet, descend.
                        w.scc_aux.dfs_pre_num = next_dfs_pre_num
                        w.scc_aux.lowlink = next_dfs_pre_num
                        next_dfs_pre_num += 1
                        scc_stack.append(w)
                        w.scc_aux.on_scc_stack = True
                        walk_stack.append((v, v_edges_iter, ))
                        v = w
                        v_edges_iter = iter(v._out_edges)
                        continue
                    elif w.scc_aux.dfs_pre_num < v.scc_aux.dfs_pre_num:
                        # Frond or cross-link.
                        if w.scc_aux.on_scc_stack:
                            v.scc_aux.lowlink = min(v.scc_aux.lowlink,
                                                    w.scc_aux.dfs_pre_num)
                else:
                    # v's edges exhausted
                    if v.scc_aux.lowlink == v.scc_aux.dfs_pre_num:
                        # v is root of component
                        # First mark all edges between nodes in the component
                        # as such.
                        for w in reversed(scc_stack):
                            if w.scc_aux.dfs_pre_num < v.scc_aux.dfs_pre_num:
                                break
                            for edge in w._out_edges:
                                u = edge.spawn
                                if (u.scc_aux.on_scc_stack and
                                    (u.scc_aux.dfs_pre_num >=
                                     v.scc_aux.dfs_pre_num)):
                                    edge.is_scc_edge = True
                                else:
                                    edge.is_scc_edge = False
                        # And collect the nodes.
                        sc_component = []
                        while scc_stack:
                            w = scc_stack[-1]
                            if w.scc_aux.dfs_pre_num < v.scc_aux.dfs_pre_num:
                                break
                            scc_stack.pop()
                            w.scc_aux.on_scc_stack = False
                            sc_component.append(w)
                        assert(sc_component)
                        if len(sc_component) == 1:
                            sc_component[0].sc_component = None
                        else:
                            for w in sc_component:
                                w.sc_component = sc_component
                            sc_components.append(sc_component)

                        # Required by the edge marking above.
                        assert(all(w.scc_aux.dfs_pre_num < v.scc_aux.dfs_pre_num
                                   for w in scc_stack))

                    # In either case, move up on the walk stack
                    w = v
                    v = None
                    if walk_stack:
                        (v, v_edges_iter,) = walk_stack.pop()
                        v.scc_aux.lowlink = min(v.scc_aux.lowlink,
                                                w.scc_aux.lowlink)

        self._sc_components = sc_components;
        for node in self._nodes:
            del(node.scc_aux)

    def find_spawns_recursive(self, name):
        if not name in self._nodes_by_name:
            return None

        worklist = []
        def enqueue_node_to_wl(node):
            if node.sc_component is None:
                worklist.append(node)
                return
            # Do not consider loops involving the entry root and
            # always circling back without ever leaving the strongly
            # connected component. Find all nodes in the strongly
            # connected component which have outgoing edges leaving
            # the component. Recursively traceback their incoming
            # edges within the component all the way back to the entry
            # node.  Note that in a strongly connected component, all
            # nodes are reachable in the forward-direction by any
            # other node, the entry node in particular. So walking
            # incoming edges backwards will always result in some
            # (reverse) path from the entry to the node in question.
            for v in node.sc_component:
                v.on_scc_worklist = False
            scc_worklist = list(filter(lambda v:
                                        (v != node and
                                         (not v.removed or
                                          any(not edge.is_scc_edge
                                              for edge in v._out_edges))),
                                       node.sc_component))
            for v in scc_worklist:
                v.on_scc_worklist = True
            node.on_scc_worklist = True
            worklist.append(node)
            while scc_worklist:
                v = scc_worklist.pop()
                worklist.append(v)
                for edge in filter(lambda e: e.is_scc_edge, v._in_edges):
                    if edge.original.on_scc_worklist:
                        continue
                    scc_worklist.append(edge.original)
                    edge.original.on_scc_worklist = True
            for v in node.sc_component:
                del(v.on_scc_worklist)

        for node in self._nodes_by_name[name]:
            enqueue_node_to_wl(node)

        found_spawns = {}
        while worklist:
            node = worklist.pop()
            for edge in node._out_edges:
                # Strongly connected components get batched as a whole,
                # skipping SCC edges here is sufficient to avoid cycling.
                if edge.is_scc_edge:
                    continue
                enqueue_node_to_wl(edge.spawn)

            # Follow paths across removed IPA optimization clones,
            # but don't record them.
            if node.removed and node.is_ipa_opt_clone():
                continue

            if node.node_id.name in found_spawns:
                found_spawns[node.node_id.name] |= node.is_ipa_opt_clone()
            else:
                found_spawns[node.node_id.name] = node.is_ipa_opt_clone()

        return found_spawns

    def all_instances_removed(self, name):
        # This is a heuristic to be used for deciding about
        # externizeability: if there's at least some entry for name,
        # and all have been removed, then chances are the function in
        # question has not been emitted to the object.
        if not name in self._nodes_by_name:
            return False

        return all(node.removed for node in self._nodes_by_name[name])
