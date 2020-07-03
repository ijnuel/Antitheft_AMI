const rootRef = firebase.database().ref("Energy_Data");

function send(user, voltage, current, power, energy_available, energy_used, user2, voltage2, current2, power2, energy_available2, energy_used2) {
    document.getElementById("button").addEventListener("click", function () {
        var energy_added = document.getElementById("Add_Energy").value;
        console.log(energy_available);
        console.log(energy_added);
        energy_available = parseInt(energy_available, 10) + parseInt(energy_added, 10);
        console.log(energy_available);
        rootRef.child("User_1").set({
            "voltageInput": voltage,
            "current": current,
            "power":power,
            "energyConsumed": energy_used,
            "energyAvailable": energy_available
            }
        )
            .then(function(){
                document.getElementById("Add_Energy").value = "";
            })
    });

    document.getElementById("button2").addEventListener("click", function () {
        var energy_added2 = document.getElementById("Add_Energy2").value;
        console.log(energy_available2);
        console.log(energy_added2);
        energy_available2 = parseInt(energy_available2, 10) + parseInt(energy_added2, 10);
        console.log(energy_available2);
        rootRef.child("User_2").set({
                "voltageInput": voltage2,
                "current": current2,
                "power":power2,
                "energyConsumed": energy_used2,
                "energyAvailable": energy_available2
            }
        )
            .then(function(){
                document.getElementById("Add_Energy2").value = "";
            })
    });


    $('#User').html(user);
    $('#V').html(`${voltage} V`);
    $('#I').html(`${current} A`);
    $('#P').html(`${power*1000} W`);
    $('#EU').html(`${energy_used} kWs`);
    $('#EA').html(`${energy_available} kWs`);
    $('#User2').html(user2);
    $('#V2').html(`${voltage2} V`);
    $('#I2').html(`${current2} A`);
    $('#P2').html(`${power2*1000} W`);
    $('#EU2').html(`${energy_used2} kWs`);
    $('#EA2').html(`${energy_available2} kWs`);
}

$(document).ready(function () {
    rootRef.orderByKey().limitToLast(2).on("value", function(snapshot) {
        const volt = (snapshot.val());
        const arr = Object.keys(volt).map(key => volt[key]);
        const x = arr[0];
        const e_data = Object.keys(x).map(key => x[key]);
        const user = snapshot.child("/User_1").key;
        const user2 = snapshot.child("/User_2").key;
        const current = e_data[0];
        const energy_available = e_data[1];
        const energy_used = e_data[2];
        const power = e_data[3];
        const voltage = e_data[4];
        const x2 = arr[1];
        const e_data2 = Object.keys(x2).map(key => x2[key]);
        const current2 = e_data2[0];
        const energy_available2 = e_data2[1];
        const energy_used2 = e_data2[2];
        const power2 = e_data2[3];
        const voltage2 = e_data2[4];
        console.log(volt);
        console.log(user);
        console.log(user2);
        console.log(current);
        console.log(energy_available);
        console.log(energy_used);
        console.log(power);
        console.log(voltage);
        console.log(current2);
        console.log(energy_available2);
        console.log(energy_used2);
        console.log(power2);
        console.log(voltage2);
        send(user, voltage, current, power, energy_available, energy_used, user2, voltage2, current2, power2, energy_available2, energy_used2);
    }, function (errorObject) {
        console.log("The read failed: " + errorObject.code);
    });
});