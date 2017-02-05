All based on instructions from http://www.andybev.com/index.php/Using_iptables_and_PHP_to_create_a_captive_portal

# Install Software
* Install dnsmasq
* Install hostapd
 * Make sure the config file is configured properly in /etc/init.d/hostapd (see example in repo)
* Install dhcpd
 * apt-get install isc-dhcp-server
* Install nginx
 * Make sure there are no other webservers on port 80 (e.g. Kura)
* Install php5-fpm

# Configuration
* Configure network interfaces
 * configure wlan0 with a static IP address
 * restore iptables on as pre-up config

* Configure DHCP server on wlan0

* Configure iptables
 * Store config as /etc/iptables.rules

*Setup website
 * Config for default webserver including redirects for the various device manufacturers
   * google.php to generate 204 return code for /generate_204
 * Config for virtual host with registration page (see default configuration in available-sites)
 * Database for usernames and mac addresses is located in www/db/, this directory needs to be writable by the nginx user (www-data)
