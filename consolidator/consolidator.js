var http = require('http');
var fs = require('fs');
var url = require('url');
var mqtt = require('mqtt');

var sensor_env = {};
var sensor_wifi = {};

var mqtt_topic_base = 'protiotype/protiotype_1';

var mqtt_url = url.parse('mqtt://iot.eclipse.org:1883');
var auth = (mqtt_url.auth || ':').split(':');

var options = {
port: mqtt_url.port,
      clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
      username: auth[0],
      password: auth[1],
};

// Create a client connection
var client = mqtt.connect(mqtt_url, options);

client.on('connect', function() {
		client.subscribe(mqtt_topic_base + '/#', function() {
				client.on('message', function(topic, msg, err) {
						var t = topic.substring(mqtt_topic_base.length, topic.length);
						if (t == "/clients") {
						sensor_wifi = JSON.parse(msg);
						console.log('Receive MQTT message: ' + JSON.stringify(sensor_wifi));
						} else if (t == "/environment") {
						sensor_env = JSON.parse(msg);
						console.log('Receive MQTT message: ' + JSON.stringify(sensor_env));
						}
						});
				});
		});

client.on('error', function() {
		http.createServer( function (req, res) {

				var pathname = url.parse(req.url).pathname;
				var body_json = {};

				if (req.method == 'POST')
				{
				console.log('Received a POST request');
				var body = '';
				req.on('data', function(data) {
						body += data;
						});
				req.on('end', function() {
						body_json = JSON.parse(body);
						if (pathname == '/environment')
						{
						sensor_env = body_json;
						console.log('Environment values: ' + JSON.stringify(sensor_env));
						}
						else if (pathname == '/clients')
						{
						sensor_wifi = body_json;
						console.log('Clients: ' + JSON.stringify(sensor_wifi));
						}
						else if (pathname == '/clients/event')
						{
						console.log('Event: ', JSON.stringify(body_json));
						}
						});
				}

				if (req.method == 'GET')
				{
					console.log('GET: Handling request for ' + pathname);

					if (pathname == "/environment")
					{
						console.log("Received get for environment data: " + JSON.stringify(sensor_env));
						res.writeHead(200, {'Content-Type': 'text/json'});
						res.write(JSON.stringify(sensor_env));
						res.end();
					}
					else if (pathname == '/clients')
					{
						console.log("Received get for client data: " + JSON.stringify(sensor_wifi));
						res.writeHead(200, {'Content-Type': 'text/json'});
						res.write(JSON.stringify(sensor_wifi));
						res.end();
					}
				}		

		}).listen(8000);
});
