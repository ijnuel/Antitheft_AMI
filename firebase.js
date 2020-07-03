const firebaseConfig = {
    apiKey: "AIzaSyARziVJseo4piiRp9zZgi1QPsBkFGHPt8A",
    authDomain: "advancedmeteringinfrastr-901c3.firebaseapp.com",
    databaseURL: "https://advancedmeteringinfrastr-901c3.firebaseio.com",
    projectId: "advancedmeteringinfrastr-901c3",
    storageBucket: "advancedmeteringinfrastr-901c3.appspot.com",
    messagingSenderId: "851606767960",
    appId: "1:851606767960:web:ece290e2e0406b5d44bd2d",
    measurementId: "G-11JWTHMB68"
};

firebase.initializeApp(firebaseConfig);
$(document).ready(function () {
    try {
        let app = firebase.app();
        let features = ['database'].filter(feature => typeof app[feature] === 'function');
        $('#load').html(`Firebase SDK loaded with ${features.join(', ')}`);
    } catch (e) {
        console.error(e);
        document.getElementById('load').innerHTML = 'Error loading the Firebase SDK, check the console.';
    }
});