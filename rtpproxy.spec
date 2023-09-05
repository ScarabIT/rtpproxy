Name:		rtpproxy
Version:	2.2.1
Release:	2
Summary:	A symmetric RTP proxy
License:        BSD
URL:		http://www.rtpproxy.org
Source0:    %{name}-%{version}.tar.gz

BuildRequires:	autoconf
BuildRequires:	automake
BuildRequires:	bcg729-devel
BuildRequires:	docbook-style-xsl
BuildRequires:	gcc
BuildRequires:	gsm-devel
BuildRequires:	libsndfile-devel
BuildRequires:	libsrtp-devel
BuildRequires:	libtool
BuildRequires:	libxslt
BuildRequires:	make
BuildRequires:	systemd-devel
BuildRequires:  systemd-rpm-macros
Requires(pre):	/usr/sbin/useradd
Requires(post): systemd
Requires(preun): systemd
Requires(postun): systemd


%description
This is symmetric RTP proxy designed to be used in conjunction with
the SIP Express Router (SER) or any other SIP proxy capable of
rewriting SDP bodies in SIP messages that it processes.


%prep
%autosetup -p1

%build
autoreconf -ivf
%configure --enable-systemd --disable-static
%make_build
make rtpproxy.8


%install
%make_install
find $RPM_BUILD_ROOT -name "*.la" -delete
install -D -p -m 0644 rpm/%{name}.sysconfig %{buildroot}%{_sysconfdir}/sysconfig/%{name}
# install systemd files
install -D -m 0644 -p rpm/%{name}.service %{buildroot}%{_unitdir}/%{name}.service
install -D -m 0644 -p rpm/%{name}.tmpfiles.conf %{buildroot}%{_tmpfilesdir}/%{name}.conf
#mkdir -p %{buildroot}%{_localstatedir}/run/%{name}
install -d %{buildroot}%{_localstatedir}/lib/%{name}


%pre
getent passwd %{name} >/dev/null || \
/usr/sbin/useradd -r -c "RTPProxy service"  -d %{_localstatedir}/lib/%{name} -s /sbin/nologin %{name} 2>/dev/null || :


%post
%systemd_post %{name}.service


%preun
%systemd_preun %{name}.service


%files
%doc AUTHORS README.md README.remote
%license LICENSE
%config(noreplace) %{_sysconfdir}/sysconfig/%{name}
%{_unitdir}/%{name}.service
%{_tmpfilesdir}/%{name}.conf
#%dir %attr(0755, rtpproxy, rtpproxy) %{_localstatedir}/run/%{name}
%{_bindir}/extractaudio
%{_bindir}/makeann
%{_bindir}/rtpproxy
%exclude %{_bindir}/rtpproxy_debug
%ifarch %{ix86} x86_64
# requires rdtsc which is available only on x86/x86_64 arches
%{_bindir}/udp_contention
%endif
%{_libdir}/rtpproxy/*.so
%{_mandir}/man8/rtpproxy.8*
%dir %attr(0750, rtpproxy, rtpproxy) %{_localstatedir}/lib/%{name}


%changelog
* Tue Sep 05 2023 Luis Leal <luisl@scarab.co.za> 2.2.1-2
- Packaging fixes (luisl@scarab.co.za)
* Mon Sep 04 2023 Luis Leal <luisl@scarab.co.za>
- new package built with tito

