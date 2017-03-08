import Ember from 'ember';
import ENV from 'beermon-v2/config/environment';

export default Ember.Mixin.create({
    conn_timeout: 3,
    status: 'disconnected',
    client: new Paho.MQTT.Client(ENV.APP.MQTT_BROKER.addr, ENV.APP.MQTT_BROKER.port, "beermon_" + parseInt(Math.random() * 100, 10)),
    init() {
        this._super();
        this.get('client').onConnectionLost = $.proxy(this.get('_on_connection_lost'), this);
        this.get('client').onMessageArrived = $.proxy(this.get('_on_message_arrived'), this);
        this.get('client').connect({
            timeout: this.get('conn_timeout'),
            onSuccess: $.proxy(this.get('_on_connection_success'), this),
            onFailure: $.proxy(this.get('_on_connection_failure'), this)
        });
    },
    _subscribe: Ember.observer('status', 'topics', function () {
        if (this.get('status') != 'connected') return;
        this.get('topics').forEach(x => this.get('client').subscribe(x.get('chan'), x.get('qos')));
    }),
    _on_connection_lost(rsp) {
        console.log("connection lost: " + rsp.errorMessage);
    },
    _on_message_arrived(msg) {
        this.get('messages').addObject({ topic: msg.destinationName, msg: msg.payloadString });
        console.log(msg.destinationName, ' -- ', msg.payloadString);
    },
    _on_connection_success() {
        console.log('mqtt connected');
        this.set('status', 'connected');
    },
    _on_connection_failure(rsp) {
        console.log('connection failed: ' + rsp.errorMessage);
        this.set('status', 'disconnected');
    }
});
