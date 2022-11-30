// Initialize and add the map
function initMap() {
    console.log("Hello");
    // The location of Uluru
    const uluru = { lat: 48.715861, lng: 2.211261 };
    // The map, centered at Uluru
    const map = new google.maps.Map(document.getElementById("map"), {
      zoom: 16,
      center: uluru,
    });
    // The marker, positioned at Uluru
    const marker = new google.maps.Marker({
      position: uluru,
      map: map,
    });
  }
  
  window.initMap = initMap;