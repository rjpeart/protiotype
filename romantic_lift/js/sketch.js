var partyBool;
var romanceBool;

function setup(){
	colorMode(RGB, 255);
	 
	 partyBool = false;
	 romanceBool = false;

	canvas = createCanvas(windowWidth, windowHeight);
		
	
	//center the point and give is a diameter of 1
	x = windowWidth/2;
	y = windowHeight/2;

	//initiate objects
	background(0,0,0);

	petals = createVideo(['imgs/petals.mp4']);

	waterfall = createVideo(['imgs/waterfall.mp4']);
	// waterfall.loop();
	// waterfall.hide();

};

function draw(){

console.log("PETALS " + partyBool);
console.log("WATERFALL " + romanceBool);

textSize(32);
fill(255, 0, 0);
	
if(partyBool == true){
	//romanceBool = false;
	push();
	petals.loop();
	image(petals,0,0, windowWidth, windowHeight);
	translate(x, y);
	textAlign(CENTER);
	text("Romantic Liason", 10, 30);
	pop();

} else if(romanceBool == true){
	background(0,0,0);
	push();
	waterfall.loop();
	image(waterfall,0,0, windowWidth, windowHeight);
	translate(x, y);
	textAlign(CENTER);
	text("I'm so lonely", 10, 30);
	pop();
}

else {
	background(0,0,0);
	push();
	translate(x, y);
	textAlign(CENTER);
	text("NOTHING 2C HERE", 10, 30);
	pop();
};

};

function keyPressed(){

	if(keyCode == 49){
		romanceBool = false;
		partyBool ^= true;

	} else if (keyCode == 50){
		partyBool = false;
		romanceBool ^= true;

	};

	return false;
};



