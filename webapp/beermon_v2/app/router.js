import Ember from 'ember';
import config from './config/environment';

const Router = Ember.Router.extend({
  location: config.locationType,
  rootURL: config.rootURL
});

Router.map(function() {
  this.route('beermon', function () {
    this.route('devices');
    this.route('device', { path: '/device/:id'});
  });
});

export default Router;
