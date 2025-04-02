
Name:		klp-ccp
Version:	0.0.1
Release:	6
Summary:	SUSE klp-ccp
License:	GPL-2.0
Group:		Development/Tools/Building
Url:		https://github.com/SUSE/klp-ccp
Source:	klp-ccp-%{version}.tar.gz
Requires:	python311
BuildRequires:	python311-devel
BuildRequires:	make
BuildRequires:	gcc-c++
BuildRequires:	pkgconfig

%description
The SUSE klp-ccp utility to aid the kernel livepatch creation process
with source code extraction related tasks.


%prep
%setup -q


%build
# Override to remove Werror on Tumbleweed.
%global optflags -O2
%configure
# No %%{?_smp_mflags} on purpose, memory consumption is huge.
make


%install
%make_install

%global src_suse_klp_policy examples/policies/suse
%global dst_suse_klp_policy %{_libdir}/klp-ccp/lp-creation-python-policy/suse
install -d %{buildroot}%{_libdir}/klp-ccp/lp-creation-python-policy/suse
for f in			\
	__init__.py		\
	ipa_clones.py		\
	klp_policy.py		\
	mod_symvers.py		\
	target_mod_elf.py
do
    install -m 644 "%{src_suse_klp_policy}/$f" \
	    "%{buildroot}/%{dst_suse_klp_policy}/"
done

%files
%{_bindir}/klp-ccp
%dir %{_libdir}/klp-ccp
%dir %{_libdir}/klp-ccp/lp-creation-python-policy
%{_libdir}/klp-ccp/lp-creation-python-policy/ccp.py


%package policy-suse-klp
Summary:	SUSE klp-ccp kernel livepatch policy
Requires:	klp-ccp == %{version}
Requires:	python311-pyelftools

%description policy-suse-klp
klp-ccp policy implementation used by SUSE for creating kernel
livepatches.

%files policy-suse-klp
%dir %{dst_suse_klp_policy}
%{dst_suse_klp_policy}/__init__.py
%{dst_suse_klp_policy}/ipa_clones.py
%{dst_suse_klp_policy}/klp_policy.py
%{dst_suse_klp_policy}/mod_symvers.py
%{dst_suse_klp_policy}/target_mod_elf.py
