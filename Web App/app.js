import { initializeApp } from "https://www.gstatic.com/firebasejs/9.22.0/firebase-app.js";
import { getDatabase, ref, get } from "https://www.gstatic.com/firebasejs/9.22.0/firebase-database.js";

// Target List
let targets = [[2, 2], [5, 3], [7, 2], [11, 1]];
// let tarY = [3,2,3,0,2];

// Obstacle List
let obstacles = [[1, 1], [6, 1], [10, 2]];
// let obsY = [2, 1, 1, 2];

let data;


const firebaseConfig = {
    apiKey: "AIzaSyCWf5cM5vOoN8jb2SdAz3y0lafvtZYa798",
    authDomain: "data-receiver-1913.firebaseapp.com",
    databaseURL: "https://data-receiver-1913-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "data-receiver-1913",
    storageBucket: "data-receiver-1913.appspot.com",
    messagingSenderId: "130194473971",
    appId: "1:130194473971:web:badede63053720bdd14886"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
let count = 0;

function fetchDB() {
    console.log(count++);
    // Reference to your database path
    let database = getDatabase(app);
    let dbRef = ref(database, "/");

    // Read data once
    get(dbRef).then((snapshot) => {
        data = snapshot.val();
        // console.log(typeof data);
        // console.log(data.length);
        // data['Nodelist'].forEach((i) => { console.log(i); });
        // let dataContainer = document.getElementById("dataContainer");
        // dataContainer.innerHTML = JSON.stringify(data["data"]);
        cardsCreation();
    }).catch((error) => {
        console.error(error);
    });

}


function cardsCreation() {
    let cardContainer = document.getElementById('cardContainer');
    cardContainer.innerHTML = "";
    let arena = document.getElementById('arena');
    arena.innerHTML = "";
    // Creating background glass effect
    let glass = document.createElement('div');
    glass.className = 'glass';
    arena.appendChild(glass);
    setTarObs();
    if (data.hasOwnProperty('Nodelist')) {
        data['Nodelist'].forEach(function (id) {
            // Create card element
            // console.log(data["data"][id]["x"]);
            let card = document.createElement('div');
            card.className = 'card';
            card.style.position = "relative";

            // Creating background glass effect
            let glass = document.createElement('div');
            glass.className = 'glass';
            card.appendChild(glass);

            // Create image element
            let image = document.createElement('div');
            image.className = 'card-image';
            card.appendChild(image);

            // Create title element
            let title = document.createElement('h3');
            title.className = 'title';
            title.textContent = 'ID: ' + id;
            card.appendChild(title);

            // Create coordinates element
            let coordinates = document.createElement('p');
            coordinates.className = 'coordinates';
            card.appendChild(coordinates);

            // Create active tag element
            let activeTag = document.createElement('span');
            activeTag.className = 'active-tag';
            activeTag.textContent = 'Active';
            card.appendChild(activeTag);

            // Append card to the container
            cardContainer.appendChild(card);
            // console.log(`ID searched: ${id}`);
            if (data.hasOwnProperty('data'))
                if (data["data"].hasOwnProperty(id)) {
                    // console.log(`ID Found: ${id}`);
                    // console.log(`X: ${data["data"][id]["0"]}, Y: ${data["data"][id]["1"]}`);
                    coordinates.innerHTML = `X: ${data["data"][id]["0"]}, Y: ${data["data"][id]["1"]}`;
                    // Creating Robots in arena
                    let robot = document.createElement('div');
                    robot.className = 'robot';
                    arena.appendChild(robot);
                    let x = data["data"][id]["0"] * 100 + 'px';
                    let y = data["data"][id]["1"] * 100 + 'px';
                    robot.style.right = x;
                    robot.style.bottom = y;

                    // tootltip
                    let tooltip = document.createElement('span');
                    tooltip.className = 'tooltip';
                    tooltip.innerHTML = id;
                    robot.appendChild(tooltip);

                    // robot.onmouseover = () => {
                    //     tooltip.style.opacity = '1';
                    // };
                    // robot.onmouseout = () => {
                    //     tooltip.style.opacity = '0';
                    // };
                }
                else {
                    coordinates.innerHTML = `X: ${null}, Y: ${null}`;
                }

        });
    }
}

function setTarObs() {
    targets.forEach(tar => {
        let arena = document.getElementById('arena');
        // Creating target elements
        let target = document.createElement('div');
        target.className = 'target';
        arena.appendChild(target);
        let x = tar[0] * 100 + 'px';
        let y = tar[1] * 100 + 'px';
        target.style.right = x;
        target.style.bottom = y;
    });

    obstacles.forEach(obs => {
        let arena = document.getElementById('arena');
        // Creating obstacle elements
        let obstacle = document.createElement('div');
        obstacle.className = 'obstacle';
        arena.appendChild(obstacle);
        let x = obs[0] * 100 + 'px';
        let y = obs[1] * 100 + 'px';
        obstacle.style.right = x;
        obstacle.style.bottom = y;
    });

}

fetchDB();
setInterval(fetchDB, 1000);
setTarObs();
