var Kismet = require('./lib/kismet.js')
var http = require('http');
var mqtt = require('mqtt');
var url = require('url');
//var getmac = require('getmac');

var clientId = 'protiotype_1'; // + getmac.getMac();
var use_mqtt = true;

var timeoutValue = 60*1000;  // timeout set to 60s
var taskPeriod = 2000;

var postPeriod = 5;

var k = new Kismet()

var clients = {};

var mqttBroker_url = url.parse('mqtt://iot.eclipse.org');
var mqtt_auth = (mqttBroker_url.auth || ':').split(':');

var mqtt_options = {
	port: mqttBroker_url.port,
	clientId: clientId,
	username: mqtt_auth[0],
	password: mqtt_auth[1]
};

var mqttClient = mqtt.connect(mqttBroker_url, mqtt_options);

mqttClient.on('error', function() {
	use_mqtt = false;
});

createEvent = function(client, eventText) {
	var event = {};
	event["event"] = eventText;
	event["mac"] = client.mac;
	event["firsttime"]  = client.firsttime;
	event["lasttime"] = client.lasttime;
	event["signal"] = client.signal;
	event["manufacturer"] = client.manufacturer;
	return event;
}

postData = function(data, topic) {
	var str_data = JSON.stringify(data);

	if (!use_mqtt) {
		var post_options = {
			host: '192.168.0.8',
			port: '8000',
			path: topic,
			method: 'POST',
			headers: {
				'Content-Type': 'text/json',
				'Content-Length': Buffer.byteLength(str_data)
			}
		};

		var post_req = http.request(post_options, function(res) {
			res.setEncoding('utf8');
			res.on('data', function(chunk) {
				console.log('Response: ' + chunk);
			});
		});

		post_req.on('error', (err) => {
			console.log("Error : " + err);
		});

		post_req.write(str_data);
		post_req.end();
	}
	else
	{
		mqttClient.publish('protiotype/' + clientId + topic, str_data);
	}
}



clientArray = function(clients) {
	var client_arr = [];
	var i = 0;
	for (var key in clients) {
		client_arr[i++] = clients[key];
	}
	return client_arr;
}

periodicTask = function(){
	var now = Date.now()
	var keys = Object.keys(clients);
	for (var i = keys.length-1; i >= 0; i--){
		var key = keys[i];
		if (now - clients[key].lasttime > timeoutValue){
			var event = createEvent(clients[key], "remove");
			postData(event, "/clients/event");
			console.log('client ' + clients[key].mac + ' disappeared, stayed ' + ((clients[key].lasttime - clients[key].firsttime)/1000) + ' seconds');
			delete clients[key];
		}	
	}
	if (postPeriod > 0)
	{
		postPeriod--;
	}
	else
	{
		var client_arr = clientArray(clients);
		postData(client_arr, '/clients');
		postPeriod = 5;
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
		
		if (!( fields.mac in clients )){
			console.log('client ' + fields.mac + ' found');
			clients[fields.mac] = {
				mac: fields.mac,
				firsttime: now,
				signal: fields.signal_dbm,
				manufacturer: fields.manuf,
			};
			var event = createEvent(clients[fields.mac], "add");
			postData(event, "/clients/event");
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
		var client_arr = clientArray(clients);
		res.end(JSON.stringify(client_arr));
	}
}).listen(9615);

