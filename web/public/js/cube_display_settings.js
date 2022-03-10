let camera, scene, renderer, mesh, new_color, percentage 
let count = 25 
init() 
animate() 

function init() {

  camera = new THREE.PerspectiveCamera(75, 200 / 200, 0.1, 10) 

  camera.position.z = 1 

  scene = new THREE.Scene() 

  const geometry = new THREE.BoxGeometry(0.5, 0.5, 0.5).toNonIndexed() 


  const material = new THREE.MeshBasicMaterial({}) 

  const positionAttribute = geometry.getAttribute('position') 

  mesh = new THREE.Mesh(geometry, material) 
  scene.add(mesh) 

  renderer = new THREE.WebGLRenderer({
    antialias: true,
    alpha: true,
    canvas: glcanvas
  }) 

  renderer.setSize(200, 200) 

  // wireframe
  const geo = new THREE.EdgesGeometry(mesh.geometry) 
  const mat = new THREE.LineBasicMaterial({
    color: 0x000000,
    linewidth: 4
  }) 
  const wireframe = new THREE.LineSegments(geo, mat) 
  wireframe.renderOrder = 1  // make sure wireframes are rendered 2nd
  mesh.add(wireframe) 
}


function animate(color) {
  document.onkeydown = function (e) {
    switch (e.keyCode) {
      case 37:
        count--
        percentage = count / 25 
        mesh.rotation.y -= 0.1 

        break 
      case 38:
        mesh.rotation.x += 0.1 
        break 
      case 39:
        mesh.rotation.y += 0.1 
        break 
      case 40:
        mesh.rotation.x -= 0.1 
        break 
    }
  }   

  renderer.render(scene, camera) 
  requestAnimationFrame(animate) 

}
