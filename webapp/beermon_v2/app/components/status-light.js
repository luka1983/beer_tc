import Ember from 'ember';

export default Ember.Component.extend({
    'light-color': Ember.computed('status', function () {
        let bck;
        let brd;
        switch (this.get('status')) {
            case 'ok':
                bck = '#449d44';
                brd = '#398439';
                break;
            case 'warn':
                bck = '#ec971f';
                brd = '#d58512';
                break;
            case 'err':
                bck = '#c9302c';
                brd = '#ac2925';
                break;
        }
        return Ember.String.htmlSafe('background-color:' + bck + ';border-color:' + brd);
    })
});
