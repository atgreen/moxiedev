Name:		moxielogic-repo
Version:	1
Release:	2
Summary:	Moxie Logic Repository Configuration

Group:		System Environment/Base
License:	MIT
URL:		http://moxielogic.org/wiki
Source0:	Moxie_Logic.repo
Source1:	RPM-GPG-KEY-MoxieLogic
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildArch:	noarch

%description
This package installs the repository GPG and repo files for the Moxie
Logic software repostory.

%prep


%build


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/etc/yum.repos.d
cp %{SOURCE0} $RPM_BUILD_ROOT/etc/yum.repos.d
mkdir -p $RPM_BUILD_ROOT/etc/pki/rpm-gpg
cp %{SOURCE1} $RPM_BUILD_ROOT/etc/pki/rpm-gpg


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root, 0755)
/etc/yum.repos.d/*.repo
/etc/pki/rpm-gpg/RPM*

%changelog
* Thu Feb 18 2010 Anthony Green <green@moxielogic.com> - 1-2
- Another build.
