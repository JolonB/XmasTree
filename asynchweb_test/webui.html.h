R"rawliteral(<!doctypehtml><title>Xmas Tree Control Panel</title><meta content=width=device-width,initial-scale=1 name=viewport><link href=data:, rel=icon><style>body{max-width:1200px;margin:0 auto;padding-bottom:25px;background-color:#f0f0f0;font-family:sans-serif}.row{display:flex;width:auto;margin:0 10%%}.col{flex:50%%;margin:5%%}.object{background-color:#a4a4a4;padding:10px;border-radius:10px;margin:5px 0;position:relative}.corruptobj{background:#d3d3d3;color:#747474}.clearfix:after{content:"";clear:both;display:table}span.fname{float:left;top:50%%;transform:translateY(-50%%);position:absolute}.object>button{border-radius:5px;padding:10px;margin:0;float:right}button.delete{background-color:#ff7961}button.add{background-color:#60ad5e}button.remove{background-color:#ffb74d}button.move{background-color:#dcdcdc}.gg-trash{box-sizing:border-box;position:relative;display:block;transform:scale(var(--ggs,1));width:10px;height:12px;border:2px solid transparent;box-shadow:0 0 0 2px,inset -2px 0,inset 2px 0;border-bottom-left-radius:1px;border-bottom-right-radius:1px;margin-top:4px}.gg-trash:after,.gg-trash:before{content:"";display:block;box-sizing:border-box;position:absolute}.gg-trash:after{background:currentColor;border-radius:3px;width:16px;height:2px;top:-4px;left:-5px}.gg-trash:before{width:10px;height:4px;border:2px solid;border-bottom:transparent;border-top-left-radius:2px;border-top-right-radius:2px;top:-7px;left:-2px}</style><body><h2>Christmas Tree Animations</h2><div class=row><div class=col><h2>Animation Files</h2> %ANIMATION_LIST_PLACEHOLDER%</div><div class=col><h2>Queue</h2> %ANIMATION_QUEUE_PLACEHOLDER%</div></div><div class=row><input id=image-file onchange=upload(this) type=file></div><script>function addf(n){var e=new XMLHttpRequest;e.open("GET","/add?file="+n,!1),e.send(),location.reload()}function deletef(n){var e=new XMLHttpRequest;e.open("GET","/delete?file="+n,!1),e.send(),location.reload()}function movef(n,e){var o=new XMLHttpRequest;o.open("GET","/move?index="+n+"&shift="+e,!1),o.send(),location.reload()}function removef(n){var e=new XMLHttpRequest;e.open("GET","/remove?index="+n,!1),e.send(),location.reload()}function upload(n){var e=new XMLHttpRequest;e.open("POST","/upload?fname="+n.files[0].name,!1),e.send(n.files[0]),location.reload()}</script>)rawliteral"