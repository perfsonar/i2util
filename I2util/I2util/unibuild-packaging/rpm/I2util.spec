#Version variables set by automated scripts
%define perfsonar_auto_version 5.0.0
%define perfsonar_auto_relnum 1
Name:           I2util
Version:        %{perfsonar_auto_version}
Release:        %{perfsonar_auto_relnum}%{?dist}
Summary:        I2 Utility Library
License:        ASL 2.0
Group:          Development/Libraries
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  gcc
BuildRequires:  autoconf
BuildRequires:  automake

%description
I2 Utility library. Currently contains:
	* error logging
	* command-line parsing
	* threading
	* random number support
	* hash table support

The error logging and command-line parsing are taken from a utility library
that is distributed with the "volsh" code from UCAR.

        http://www.scd.ucar.edu/vets/vg/Software/volsh

%prep
%setup -q

%build
./bootstrap
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
#%doc Changes LICENSE README
%license LICENSE
%doc README
%{_bindir}/*
%{_libdir}/libI2util.a
%{_mandir}/man1/*
%{_includedir}/*

%changelog
* Fri Aug 20 2010 Tom Throckmorton <throck@mcnc.org> 1.1-1
- minor spec changes only

* Fri Jan 11 2008 aaron@internet2.edu 1.0-1
- Initial RPM
