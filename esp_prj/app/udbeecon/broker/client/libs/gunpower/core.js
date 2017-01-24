/**
 * Created by gbox3d on 2014. 8. 14..
 */
var gunpower = {}

gunpower.util = {

    //현재 자기 주소얻기
    get_self_url : function() {

        var tokens = location.href.split(':');

        //console.log(tokens[0] + ':' + tokens[1]);
        if(tokens.length >= 1) {
            return (tokens[0] + ':' + tokens[1]);

        }
        else {

            var ip = location.href.split('/')[2];

            return 'http://'+ip;

        }



        /*
        if(theApp.self_url == '') {
            (function() {
                var tokens = location.href.split(':');
                console.log(tokens[0] + ':' + tokens[1]);

                theApp.self_url = tokens[0] + ':' + tokens[1];
            })();
        }
        */
    },
    getSelfIpAddress : function() {

        var tokens = location.href.split(':');

        var self_url = tokens[1];

        //console.log(self_url);

        self_url = self_url.substring(2,self_url.length);

        var find_index = self_url.indexOf('/');

        if(find_index >=0 ) {
            return self_url.substring(0,self_url.indexOf('/',3) );
        }
        return self_url;
    },
    removeAllChild : function(element) {
        while (element.firstChild) {
            element.removeChild(element.firstChild);
        }
    },
    insertAfter : function (newNode, referenceNode) {
        referenceNode.parentNode.insertBefore(newNode, referenceNode.nextSibling);
    },
    date : {

        //js 의 Date 객체를 mysql Time string으로 변환하기
        convertMysqlDBString : function(baseDate) {
            var yyyy = baseDate.getFullYear().toString();
            var mm = (baseDate.getMonth()+1).toString(); // getMonth() is zero-based
            var dd = baseDate.getDate().toString();
            var hh = baseDate.getHours().toString();
            var nn = baseDate.getMinutes().toString();
            var ss = baseDate.getSeconds().toString();

            //console.log( yyyy+ '-' + (mm[1]?mm:"0"+mm[0]) +'-' + (dd[1]?dd:"0"+dd[0]) + ' ' + (hh[1]?hh:"0"+hh[0]) + ' ' + (nn[1]?nn:"0"+nn[0]) ) ; // padding
            return ( yyyy+ '-' + (mm[1]?mm:"0"+mm[0]) +'-' + (dd[1]?dd:"0"+dd[0])
                + ' ' + (hh[1]?hh:"0"+hh[0])
                + ':' + (nn[1]?nn:"0"+nn[0])
                + ':' + (ss[1]?ss:"0"+ss[0])

                ) ; // padding
        },
        subHour : function(srcDate,sub_hour) {

            return new Date(srcDate.getTime() - (1000*60*60*sub_hour));
        },
        //타임존 적용하기
        applyTimeZone : function(srcTime) {

            var _time =  new Date( srcTime);

            _time += (new Date((_time.getTimezoneOffset()*60 * 1000)));

            return _time;

        }


    },
    sceneObject : {
        //씬노드 추가
        DummyTarget : function (option) {

            var object = new THREE.Group();
            var root = new THREE.Group();

            var geometry = new THREE.CubeGeometry(1.0,1,0.2);
            var material = new THREE.MeshStandardMaterial(
                {
                    color: 0xffffff
                }
            );

            var node = new THREE.Mesh(geometry, material);
            node.position.y = 0.5;
            root.add(node);


            var size = option.fontSize;
            //var text = "1";
            var canvas = document.createElement("canvas");
            canvas.width = 128;
            canvas.height = 128;

            var texture = new THREE.Texture(canvas);
            var material = new THREE.MeshBasicMaterial({
                map : texture,
                transparent : true //투명 컬러값 적용시킴

            });
            var mesh = new THREE.Mesh(new THREE.PlaneGeometry(1,1), material);
            // mesh.overdraw = true;
            mesh.doubleSided = true;
            mesh.position.x = 0;
            mesh.position.y = 0;
            mesh.position.z = 0.12;
            node.add(mesh);

            //var scope = this;
            this.fontColor = option.fontColor
            this.backGroundColor = option.backGroundColor;

            this.drawText = (function (text) {
                var context = canvas.getContext("2d");
                context.fillStyle = this.backGroundColor//'transparent'//"rgb(80,80,80)";
                context.fillRect(0, 0, canvas.width, canvas.height);
                context.font = size + "pt Arial";
                context.textAlign = "center";
                context.textBaseline = "middle";
                context.fillStyle = this.fontColor//"rgba(255,255,0,0.5)";
                context.fillText(text, canvas.width / 2, canvas.height / 2);
                context.strokeStyle = "black";
                texture.needsUpdate = true;
            }).bind(this)

            this.fallDown = function () {
                new TWEEN.Tween( root.rotation )
                    .to({x:THREE.Math.degToRad(-90)},1000)
                    .easing( TWEEN.Easing.Elastic.Out )
                    .start();
            }

            this.standUp = function () {
                new TWEEN.Tween( root.rotation )
                    .to({x:THREE.Math.degToRad(0)},1000)
                    .easing( TWEEN.Easing.Elastic.InOut )
                    .start();
            }
            var movTween = null;
            var movTween2 = null;
            this.startMoving = function (option) {

                var back_time = root.position.x * 100;
                movTween =  new TWEEN.Tween(root.position)
                    .to({x:0},back_time)
                    .easing( TWEEN.Easing.Linear.None )

                movTween2 = new TWEEN.Tween( root.position )
                    .to({x:option.dist},3000)
                    .repeat(Infinity)
                    .yoyo(true)
                    .easing( TWEEN.Easing.Linear.None );

                movTween.chain(movTween2);
                movTween.start();

            }

            this.stopMoving = function () {
                console.log('stop')
                if(movTween) {
                    console.log('stop')
                    movTween.stop();
                    movTween2.stop();
                    movTween = null;
                }

            }

            object.add(root)

            this.object = object;

        }

    }

}