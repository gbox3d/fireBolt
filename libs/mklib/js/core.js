/**
 * Created by gbox3d on 2014. 1. 18..
 *
 * mklib core
 *
 */


var mk = {}

mk.control = function (option) {

    var theApp = option.app;

    var page_history = [];
    var current_page = option.startPage;
    current_page.style.display = 'block';

    this.changePage = function(newpage) {

        page_history.push(current_page);
        current_page.style.display = 'none';

        newpage.style.display = 'block';
        current_page = newpage;
    }
    this.backPage = function() {

        current_page.style.display = 'none';

        if(page_history.length > 0 ) {
            var newpage = page_history.pop();

            newpage.style.display = 'block';
            current_page = newpage;
        }
    }

    this.OnButton = function(option,e) {

        option.target.classList.add('pressed');

        setTimeout((function() {
            option.target.classList.remove('pressed');

            option.complete();

            //theApp.mkControl.changePage(theApp.UIElement.pageIntro);

        }).bind(this),300);

    }

    this.getCurrentPage = function() {
        return current_page;
    }

}


mk.helper = {

    setUpButton : function (option) {

        var _OnButtonDown = function(e) {

            // function(e) {
            this.classList.add('pressed');

            setTimeout((function() {
                this.classList.remove('pressed');

                option.OnClick();
                //theApp.mkControl.changePage(theApp.UIElement.pageIntro);

            }).bind(this),300);
            //}

        }

        //document.querySelector('#btn-intro')

        option.target.addEventListener('mousedown',
            _OnButtonDown
        );

        option.target.addEventListener('touchstart',
            _OnButtonDown
        );


    }

}