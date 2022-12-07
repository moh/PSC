var map;
var add_target = false;
var remove_target = false;
var target_added = false;
var target = null;

// Initialize and add the map
function initMap() {
  console.log("Hello");
  // The location of Uluru
  const uluru = { lat: 48.715861, lng: 2.211261 };
  // The map, centered at Uluru
  map = new google.maps.Map(document.getElementById("map"), {
    zoom: 16,
    center: uluru,
  });
  //marker.addListener('click', test_mark);
  map.addListener('click', map_clicked);
}

/**
 * recenter the map to the position of the remote device 
 */
function map_center(){
  lat = document.querySelector('[data-gps="lat"]').dataset.value;
  lon = document.querySelector('[data-gps="lon"]').dataset.value;
  if (lat != 0 && lon != 0){
    map.setCenter({lat : parseFloat(lat), lng : parseFloat(lon)});
    map.setZoom(16);
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
    target.setMap(null);
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
  console.log(target);

  socket.send(JSON.stringify({
    type : "target",
    target : {lat : target.position.lat(), lng : target.position.lng()}
  }));
}

/**
 * function that is called when map is clicked, 
 * it will add a target if it is enabled
 * @param {*} ele the element representing the click event
 */
function map_clicked(ele){
  if (add_target && (target == null)){
    target = new google.maps.Marker({
      position: {lat:ele.latLng.lat(), lng : ele.latLng.lng()},
      map: map,
    });
    add_target = false;
  }
}


window.initMap = initMap;