var Kismet = require('./lib/kismet.js')

var k = new Kismet()

// only a "connect" event, not ready until "ready"
k.on('connect',function(){
    console.log('connected!')
});

k.on('ready',function(){
    console.log('ready!')
    this.subscribe('client'
        , ['bssid','mac','type']
        , function(had_error,message){
            console.log('client - '+ message)
    });
});

k.on('CLIENT',function(fields){
    if( fields.bssid != fields.mac ){
        console.log(
            'Kismet sees client: ' + fields.bssid
            + ' type: ' + k.types.lookup('client',fields.type)
            + ' mac: ' + fields.mac +' '+ JSON.stringify(fields)
        )
    }
});

k.connect()

