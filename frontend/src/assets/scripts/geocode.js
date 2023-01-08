export async function getCoords (address) {
  const coords = {
    lat: 0,
    lon: 0
  }

  const key = '29b2d274041e4b9c85016ebb84436ff3' // HMMMMMMM
  const apiPath = 'https://api.opencagedata.com/geocode/v1/json'
  const path = apiPath + '?' +
    'key=' + key +
    '&q=' + encodeURIComponent(address)

  // see full list of required and optional parameters:
  // https://opencagedata.com/api#forward

  const request = new XMLHttpRequest()
  request.open('GET', path, false)

  request.onload = function () {
    if (request.status === 200) {
      const data = JSON.parse(request.responseText)
      coords.lat = data.results[0].geometry.lat
      coords.lon = data.results[0].geometry.lng
    } else if (request.status <= 500) {
      console.log('unable to geocode! Response code: ' + request.status)
      const data = JSON.parse(request.responseText)
      console.log('error msg: ' + data.status.message)
    } else {
      console.log('server error')
    }
  }

  request.onerror = function () {
    console.log('unable to connect to server')
  }

  request.send()

  return coords
}
