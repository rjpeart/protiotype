<?php

$server_name = "ap";
$domain_name = "protiotype.com";
$site_name = "Protiotype Experimental Network";

// Path to the arp command on the local server
$arp = "/usr/sbin/arp";

// The following file is used to keep track of users
$users = "/var/www/db/users";

// Check if we've been redirected by firewall to here.
// If so redirect to registration address
if ($_SERVER['SERVER_NAME']!="$server_name.$domain_name") {
  header("location:http://$server_name.$domain_name/index.php?add="
    .urlencode($_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI']));
  exit;
}

// Attempt to get the client's mac address
$mac = shell_exec("$arp -a ".$_SERVER['REMOTE_ADDR']);
preg_match('/..:..:..:..:..:../',$mac , $matches);
@$mac = $matches[0];
if (!isset($mac)) { exit; }

if (!isset($_POST['email']) || !isset($_POST['name'])) {
  // Name or email address not entered therefore display form
  ?>
  <h1>Welcome to <?php echo $site_name;?></h1>

  <p>
	Your MAC address is <?php print($matches[0]); ?>
  </p>
  To access the Internet you must first enter your details:<br><br>
  <form method='POST'>
  <table border=0 cellpadding=5 cellspacing=0>
  <tr><td>Your full name:</td><td><input type='text' name='name'></td></tr>
  <tr><td>Your email address:</td><td><input type='text' name='email'></td></tr>
  <tr><td></td><td><input type='submit' name='submit' value='Submit'></td></tr>
  </table>
  </form>
  <?php
} else {
    enable_address();
}

// This function enables the PC on the system by calling iptables, and also saving the
// details in the users file for next time the firewall is reset

function enable_address() {

    global $name;
    global $email;
    global $mac;
    global $users;

    $db = new SQLite3($users);
    if (!$db)
    {
        return;
    }

    $sql_name = $db->escapeString($_POST['name']);
    $sql_email = $db->escapeString($_POST['email']);
    $sql_mac = $db->escapeString($mac);

    $sql =<<<EOF
      CREATE TABLE IF NOT EXISTS USERS
      (MAC CHAR(20) PRIMARY KEY     NOT NULL,
       NAME           VARCHAR(30)    NOT NULL,
       EMAIL          VARCHAR(30)    NOT NULL,
       REGISTERED     DATETIME,
       LASTCONNECT    DATETIME);
EOF;

    $ret = $db->exec($sql);
    if(!$ret){
	$db->close();
	return;
    }

    $sql = 'SELECT MAC FROM USERS WHERE MAC = "$sql_mac";';
    $ret = $db->query($sql);
    if ($ret->fetchArray())
    {
	print("Updating data in database");
        $sql =<<<EOF
	    UPDATE USERS SET LASTCONNECT = DATETIME('now') WHERE MAC = "$sql_mac"; 
EOF;
    }
    else
    {
	print("Inserting data in database");
 	$sql =<<<EOF
	    INSERT INTO USERS (MAC, NAME, EMAIL, REGISTERED, LASTCONNECT)
	    VALUES ("$sql_mac", "$sql_name", "$sql_email", DATETIME('now'), DATETIME('now'));
EOF;
    }

    $ret =$db->exec($sql);
    if (!$ret)
    {
	$db->close();
	return;
    }

    $db->close();

    //file_put_contents($users,$_POST['name']."\t".$_POST['email']."\t"
    //    .$_SERVER['REMOTE_ADDR']."\t$mac\t".date("d.m.Y")."\n",FILE_APPEND + LOCK_EX);
    
    // Add PC to the firewall
    //exec("sudo iptables -I internet 1 -t mangle -m mac --mac-source $mac -j RETURN");
    // The following line removes connection tracking for the PC
    // This clears any previous (incorrect) route info for the redirection
    //exec("sudo rmtrack ".$_SERVER['REMOTE_ADDR']);

    sleep(1);
    //header("location:http://".$_GET['add']);
    exit;
}

// Function to print page header
function print_header() {

  ?>
  <html>
  <head><title>Welcome to <?php echo $site_name;?></title>
  <META HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE">
  <LINK rel="stylesheet" type="text/css" href="./style.css">
  </head>

  <body bgcolor=#FFFFFF text=000000>
  <?php
}

// Function to print page footer
function print_footer() {
  echo "</body>";
  echo "</html>";

}

?>
