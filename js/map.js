var map;
var add_target = false;
var remove_target = false;
var target_added = false;
var target = null;
var boat_marker = null;
var boat_marker_object = null;
// last location of the boat 
var last_location = null;
// The location of map at the start 
const uluru = { lat: 48.715861, lng: 2.211261 };


var boatIcon = L.icon({
  iconUrl : "img/boat.png",
  
  iconSize : [40, 40],
  iconAnchor : [20,20],
  className : "boatIcon"
});

// Initialize and add the map
function initMap() {

  // The map, centered at Uluru
  map = L.map('map',{
    center: [uluru["lat"], uluru["lng"]],
    zoom:13
  });

  L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
  maxZoom: 19,
  attribution: '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>'
  }).addTo(map);
  boat_marker = L.marker([uluru["lat"], uluru["lng"]], {icon: boatIcon}).addTo(map);
  // select boat marker html
  boat_marker_object = document.getElementsByClassName("boatIcon")[0];

  // map is clicked 
  map.on("click", map_clicked);
}


/**
 * update the marker position and angle in the map 
 */
function update_marker(){
  
  lat = document.querySelector('[data-gps="lat"]').dataset.value;
  lng = document.querySelector('[data-gps="lon"]').dataset.value;
  yaw = document.querySelector('[name="yaw_angle"]').value;
  if(lat == 0){lat = uluru["lat"];}
  if(lng == 0){lng = uluru["lng"];}
  
  var newLatLng = new L.LatLng(lat, lng);
  boat_marker.setLatLng(newLatLng);
  boat_marker_object.style.transform += " rotate(" + yaw + "deg)";

  if (last_location != null){
    l = [last_location, [lat, lng]]
    var polyline = L.polyline(l, {color: 'red'}).addTo(map);
  }
  last_location = [lat, lng];

}



/**
 * recenter the map to the position of the remote device 
 */
function map_center(){
  lat = last_location[0];
  lng = last_location[1];
  if (lat != 0 && lng != 0){
    map.setView(new L.LatLng(lat, lng), 13);
  }
}

/**
 * enable adding a target, only one target can be added
 */
function map_add_location(){
  if (!add_target){
    add_target = true;
  }
}


/**
 * remove the added target 
 */
function map_remove_location(){
  if(target != null){
    map.removeLayer(target);
  }
  target = null;
}

/**
 * send target location to remote_device 
 */
function send_target(){
  console.log("send target");
  
  if (target == null) {
    return;
  }
  if (!connected || !remote_device_connected) return;
  console.log(target);
  
  socket.send(JSON.stringify({
    type : "target",
    target : {lat : target._latlng.lat, lng : target._latlng.lng}
  }));
}

/**
 * function that is called when map is clicked, 
 * it will add a target if it is enabled
 * @param {*} ele the element representing the click event
 */
function map_clicked(ele){
  if (add_target && (target == null)){
    target = new L.marker(ele.latlng).addTo(map);

    add_target = false;
  }
}