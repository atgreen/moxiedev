Name:		moxie-quartus-jtagd
Version:	1
Release:	1%{?dist}
Summary:	Glue required to set up USB-Blaster on Fedora
Group:          Development/Tools
License:	GPLv2+
URL:		http://moxielogic.org/wiki
Source0:	quartus-jtagd
Source1:	30-usb-blaster.rules
Source3:        moxie-quartus-jtagd-README.txt
Source4:        COPYING
BuildArch:      noarch

%description
The moxie-quartus-jtagd package sets up udev rules, a quartus-jtagd
service, and moves the Altera parts description file into place.  It
assumes that Altera's Quartus II is installed under /opt/altera/.

%prep 
%setup -T -c quartus-jtagd -n quartus-jtagd

%build
cp %{SOURCE3} .
cp %{SOURCE4} .

%install
install -D -m 755 %{SOURCE0} %{buildroot}/etc/init.d/quartus-jtagd
install -D -m 644 %{SOURCE1} %{buildroot}/etc/udev/rules.d/30-usb-blaster.rules
install -d -m 755 %{buildroot}/etc/jtagd

%post
if [ ! -f /etc/jtagd/jtagd.pgm_parts ]; then
  ln -s /opt/altera/quartus/linux/pgm_parts.txt /etc/jtagd/jtagd.pgm_parts;
fi

%files
/etc/init.d/*
/etc/udev/rules.d/*
/etc/jtagd
%doc COPYING moxie-quartus-jtagd-README.txt

%changelog
* Thu Oct 27 2011 Anthony Green <green@moxielogic.com> - 11-1
- Created

