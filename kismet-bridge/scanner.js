var Kismet = require('./lib/kismet.js')
var http = require('http');

var timeoutValue = 5*1000;

var k = new Kismet()

var clients = {};

timeout = function(){
	var now = Date.now()
	var keys = Object.keys(clients);
	for (var i = keys.length-1; i >= 0; i--){
		var key = keys[i];
		if (now/1000 - clients[key].lasttime > timeoutValue){
			console.log('client ' + JSON.stringify(clients[key]) + ' disappeared');
			delete clients[key];
		}	
	}
}

// only a "connect" event, not ready until "ready"
k.on('connect',function(){
    console.log('connected!')
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
//        console.log(
//            'Kismet sees client: ' + fields.bssid
//            + ' type: ' + k.types.lookup('client',fields.type)
//            + ' mac: ' + fields.mac +' '+ JSON.stringify(fields)
//        )
	if (!( fields.mac in clients )){
		console.log('client ' + fields.mac + ' found')
	}
	clients[fields.mac] = {
          mac: fields.mac,
          firsttime: fields.firsttime,
          lasttime: fields.lasttime,
	  signal: fields.signal_dbm,
	  manufacturer: fields.manuf,
        };
   }
});

var t = setInterval(timeout, 2000)

k.connect();

http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'application/json'});
  res.end(JSON.stringify(clients));
}).listen(9615);

