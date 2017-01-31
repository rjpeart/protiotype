var Kismet = require('./lib/kismet.js')
var http = require('http');
var url = require('url');

var timeoutValue = 60*1000;  // timeout set to 60s
var taskPeriod = 2000;

var k = new Kismet()

var clients = {};

periodicTask = function(){
	var now = Date.now()
	var keys = Object.keys(clients);
	for (var i = keys.length-1; i >= 0; i--){
		var key = keys[i];
//		console.log('now = ' + now + ' lasttime = ' + clients[key].lasttime + " difference = " +  (now - clients[key].lasttime));
		if (now - clients[key].lasttime > timeoutValue){
			console.log('client ' + clients[key].mac + ' disappeared, stayed ' + ((clients[key].lasttime - clients[key].firsttime)/1000) + ' seconds');
			delete clients[key];
		}	
	}
}

// only a "connect" event, not ready until "ready"
k.on('connect',function(){
    console.log('connected!');
});

k.on('ready',function(){
    console.log('ready!')
    this.subscribe('client'
        , ['bssid','mac','type','signal_dbm','firsttime','lasttime','manuf']
        , function(had_error,message){
            console.log('client - '+ message)
    });
});

k.on('CLIENT',function(fields){
	if( fields.bssid != fields.mac ){
		var now = Date.now();
//      	console.log(
//            		'Kismet sees client: ' + fields.bssid
//            		+ ' type: ' + k.types.lookup('client',fields.type)
//            		+ ' mac: ' + fields.mac +' '+ JSON.stringify(fields)
//        	);
		if (!( fields.mac in clients )){
			console.log('client ' + fields.mac + ' found');
			clients[fields.mac] = {
	        		mac: fields.mac,
          			firsttime: now,
	  			signal: fields.signal_dbm,
	  			manufacturer: fields.manuf,
        		};
		}
		clients[fields.mac].lasttime = now;
    	}
});

var t = setInterval(periodicTask, taskPeriod);

k.connect();


http.createServer(function (req, res) {
	var params = url.parse(req.url, true);
	if ("csv" in params.query){
		res.writeHead(200, {'Content-Type': 'text/csv', 'Content-Disposition': 'inline; filename="clients.csv"'});
		res.write('mac,firsttime,lasttime,signal,manufacturer\r\n');
		for (var key in clients){
			var client = clients[key];
			res.write(client.mac + "," + client.firsttime + "," + client.lasttime + "," + client.signal + "," + client.manufacturer + "\r\n");
		}
		res.end();
	}
	else
	{
		res.writeHead(200, {'Content-Type': 'application/json'});
		res.end(JSON.stringify(clients));
	}
}).listen(9615);

