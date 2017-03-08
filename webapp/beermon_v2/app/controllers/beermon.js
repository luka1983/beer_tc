import Ember from 'ember';
import MqttAware from 'beermon-v2/mixins/mqtt-aware';

export default Ember.Controller.extend({
    fobs: Ember.observer('filter', function () {
        var f = this.get('filter');
        this.set('model.filter', f);
    })
});
