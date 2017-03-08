import Ember from 'ember';
import MqttAwareMixin from 'beermon-v2/mixins/mqtt-aware';
import { module, test } from 'qunit';

module('Unit | Mixin | mqtt aware');

// Replace this with your real tests.
test('it works', function(assert) {
  let MqttAwareObject = Ember.Object.extend(MqttAwareMixin);
  let subject = MqttAwareObject.create();
  assert.ok(subject);
});
