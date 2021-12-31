let camera2, scene2, renderer2, mesh2;
init();
animate();

function init() {

    camera2 = new THREE.PerspectiveCamera(75, 200 / 200, 0.1, 10);

    camera2.position.z = 1;

    scene2 = new THREE.Scene();

    const geometry2 = new THREE.BoxGeometry(0.5, 0.5, 0.5).toNonIndexed();


    const material2 = new THREE.MeshBasicMaterial({});

    const positionAttribute = geometry2.getAttribute('position');

    mesh2 = new THREE.Mesh(geometry2, material2);
    scene2.add(mesh2);

    renderer2 = new THREE.WebGLRenderer({
        antialias: true,
        alpha: true,
        canvas: glcanvas_homepage
    });

    renderer2.setSize(400, 400);

    // wireframe
    const geo2 = new THREE.EdgesGeometry(mesh2.geometry);
    const mat2 = new THREE.LineBasicMaterial({
        color: 0x000000,
        linewidth: 4
    });
    const wireframe2 = new THREE.LineSegments(geo2, mat2);
    wireframe2.renderOrder = 1; // make sure wireframes are rendered 2nd
    mesh2.add(wireframe2);

}


function animate() {

    requestAnimationFrame(animate);


    mesh2.rotation.x += 0.01;
    mesh2.rotation.y += 0.01;
    renderer2.render(scene2, camera2);
}


setInterval(
    function () {
      var randomColor = Math.floor(Math.random()*16777215).toString(16);
      mesh2.material.color.set(new THREE.Color("#"+randomColor));
    },1000);