import Ember from 'ember';
import MqttAware from 'beermon-v2/mixins/mqtt-aware';

export default Ember.Controller.extend(MqttAware, {
    messages: Ember.A(),
    filteredDevices: Ember.computed('model.filter', 'devices', function () {
        if (!this.get('model.filter')) return this.get('devices');
        let r = new RegExp('^.*' + this.get('model.filter') + '.*$', 'i');
        return this.get('devices').filter((x) => { return r.test(x.id) || r.test(x.desc); });
    }),
    topics: Ember.A([
        Ember.Object.create({ chan: 'devices', qos: 2 })
    ]),
    init() {
        this._super();
    },
    devices: Ember.computed('messages.[]', function () {
        var msgs = this.get('messages');
        if (msgs.length === 0) return Ember.A();
        return Ember.A(JSON.parse(msgs.objectAt(msgs.length - 1).msg));
    })
});
