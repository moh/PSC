var map;
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
    // The marker, positioned at Uluru
    const marker = new google.maps.Marker({
      position: uluru,
      map: map,
    });

    marker.addListener('click', test_mark);
    map.addListener('click', test_mark);
  }
  
  window.initMap = initMap;

  function test_mark(ele){
    console.log(ele);
    console.log(ele.latLng.lat());
    console.log(ele.latLng.lng());
    const marker = new google.maps.Marker({
      // position: {lat:ele.latLng.lat(), },
      map: map,
    });
    console.log("Marker clicked");
  }