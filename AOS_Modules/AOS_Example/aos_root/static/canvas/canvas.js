function canvasDrawStuff() {
	var canvas = document.getElementById('mycanvas');
	if (canvas.getContext){
	  var ctx = canvas.getContext('2d');
		ctx.strokeStyle = "rgb("+Math.round(Math.random()*256)+","+Math.round(Math.random()*256)+","+Math.round(Math.random()*256)+")";
		ctx.fillStyle = "rgb("+Math.round(Math.random()*256)+","+Math.round(Math.random()*256)+","+Math.round(Math.random()*256)+")";
	  ctx.beginPath();
		ctx.moveTo(Math.round(Math.random()*600),Math.round(Math.random()*400));
		ctx.lineTo(Math.round(Math.random()*600),Math.round(Math.random()*400));
		ctx.lineTo(Math.round(Math.random()*600),Math.round(Math.random()*400));
    ctx.fill();
	}
}
