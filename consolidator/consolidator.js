var http = require('http');
var fs = require('fs');
var url = require('url');

var sensor_env = {};
var sensor_wifi = {};

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
