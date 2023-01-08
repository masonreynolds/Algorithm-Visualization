<template>
  <h1 class="text" v-if="dist === 0">Traveling Salesman</h1>
  <h1 class="text" v-else>Traveling Salesman - {{ dist }} kilometers - Temp: {{ temp }}</h1>
  <br/>

  <div class="row">
    <div class="column">
      <input type="text" placeholder="Enter address or city" class="textbox" v-model="address" />
      <br/>
      <br/>
      <div class="mt-1">
        <button type="button" class="btn btn-primary mr-2" @click="handleValidPos" :disabled="running">Add</button>
        <button type="button" class="btn btn-primary mr-2" @click="clearPoses" :disabled="invalid || running">Clear</button>
        <button type="button" class="btn btn-primary mr-2" @click="runSimAnnealing" :disabled="invalid || running">Calculate Path</button>
      </div>
      <br />
      <br/>

      <svg id='globe'></svg>
    </div>

    <div class="column" id="c2">
      <h3 class="text">Max Temperature: {{ maxTemp }}</h3>
      <div class="slidecontainer">
        <input type="range" min="1" max="100000" class="slider" id="myRange" v-model="maxTemp" :disabled="running">
      </div>

      <br />
      <h3 class="text">Decrement per Step: {{ decrement }}</h3>
      <div class="slidecontainer">
        <input type="range" min="0.95" max="0.9999" step="0.0001" class="slider" id="myRange" v-model="decrement" :disabled="running">
      </div>

      <br />
      <h3 class="text">Threshold: {{ threshold }}</h3>
      <div class="slidecontainer">
        <input type="range" min="1" :max="maxTemp" class="slider" id="myRange" v-model="threshold" :disabled="running">
      </div>

      <br />
      <button type="button" class="btn btn-primary mr-2" :disabled="running" @click="switchSteps">{{ updateText }}</button>
      <br /><br />

      <h3 class="text">Locations entered: </h3>
      <p id="locations" class="text" style="white-space: pre-wrap" v-if="positions.length === 0">None so far</p>
      <span v-else>
        <p class="text" v-for="pose in positions" v-bind:key=pose.ID>
          <b>{{ pose.name }}</b>: {{ pose.lat }}, {{ pose.lon }}
        </p>
      </span>
    </div>
  </div>
</template>

<script>
import * as geoCoder from '../assets/scripts/geocode'
import * as topojson from 'topojson-client'
import * as d3 from 'd3'

export default {
  name: 'TravelingSalesman',
  data () {
    return {
      markerGroup: null,
      positions: [],
      links: [],
      svg: null,
      invalid: true,
      running: false,
      decrement: 0.999,
      address: '',
      updatesOn: true,
      updateText: 'Turn Updates Off',
      rendered: false,
      threshold: 1,
      maxTemp: 10000,
      rand: null,
      temp: null,
      timer: null,
      socketTimer: null,
      time: 2,
      projection: null,
      dist: 0,
      count: 0,
      width: 500,
      height: 500,
      offset: 230,
      path: null,
      config: {
        verticalTilt: 0,
        rotation: 90
      },
      connection: null
    }
  },
  created: function () {
    this.connection = new WebSocket('ws://localhost:18080/Simulated-Annealing-WS')

    this.connection.onmessage = async (event) => {
      const data = JSON.parse(event.data)
      this.links = data.links
      await this.updateGlobe()
      this.dist = Math.round(data.distance * 100) / 100
      this.temp = Math.round(data.temperature * 100) / 100

      if (data.last) {
        this.running = false
        this.invalid = false
      }
    }
  },
  mounted: async function () {
    this.projection = d3.geoOrthographic()
    this.path = d3.geoPath().projection(this.projection)
    this.svg = d3.select('#globe').attr('viewBox', [this.offset, 0, this.width, this.height])
    this.markerGroup = this.svg.append('g')
    await this.createGlobe()
    await this.createGlobeListeners()
    this.projection.rotate([this.config.rotation, this.config.verticalTilt, 0])
  },
  methods: {
    async createGlobeListeners () {
      document.getElementById('globe').addEventListener('mousedown', (event) => {
        if (event.button === 0) {
          if (document.getElementById('globe').onmousemove) {
            document.getElementById('globe').onmousemove = null
            this.timer.stop()
          }

          const start = this.config.verticalTilt
          const rot = this.config.rotation
          const mouseX = event.clientX
          const mouseY = event.clientY
          let dragging = true

          document.getElementById('globe').onmousemove = (event) => {
            this.config.rotation = rot - (mouseX - event.clientX) * 0.2
            this.config.verticalTilt = start + (mouseY - event.clientY) * 0.2

            if (dragging) {
              this.enableRotation()
              dragging = false
            }
          }
        } else if (event.button === 2) {
          const m = document.getElementById('globe').getScreenCTM()
          let p = document.getElementById('globe').createSVGPoint()
          p.x = event.clientX
          p.y = event.clientY
          p = p.matrixTransform(m.inverse())

          const coords = this.projection.invert([p.x, p.y])
          this.positions.push({
            id: Math.floor(Math.random() * 100000),
            lat: parseFloat(coords[1].toFixed(4)),
            lon: parseFloat(coords[0].toFixed(4)),
            name: 'Custom Location ' + (++this.count)
          })

          this.links = []
          this.invalid = this.positions.Count < 3
          this.updateGlobe()
        }
      })

      document.getElementById('globe').addEventListener('mouseup', (event) => {
        if (event.button === 0) {
          document.getElementById('globe').onmousemove = null

          if (this.timer) {
            this.timer.stop()
          }
        }
      })

      document.getElementById('globe').addEventListener('ondragstart', function () {
        return false
      })

      document.getElementById('globe').addEventListener('contextmenu', function (event) {
        event.preventDefault()
      }, false)
    },

    async enableRotation () {
      this.timer = d3.timer(() => {
        this.projection.rotate([this.config.rotation, this.config.verticalTilt, 0])
        this.svg.selectAll('path').attr('d', this.path)
        this.updateGlobe()
      })
    },

    async clearPoses () {
      this.invalid = true
      this.positions = []
      this.links = []
      this.updateGlobe()
    },

    async createGlobe () {
      this.drawGraticule()
      this.drawGlobe()
    },

    async drawGlobe () {
      d3.queue()
        .defer(d3.json, 'https://gist.githubusercontent.com/mbostock/4090846/raw/d534aba169207548a8a3d670c9c2cc719ff05c47/world-110m.json')
        .await((error, worldData) => {
          if (error) {
            console.log(error)
          } else {
            this.svg.selectAll('.segment')
              .data(topojson.feature(worldData, worldData.objects.countries).features)
              .enter().append('path')
              .attr('class', 'segment')
              .attr('d', this.path)
              .style('stroke', 'white')
              .style('stroke-width', '1px')
              .style('fill', '#222222')
              .style('opacity', '0.75')
          }
        })
    },

    async drawGraticule () {
      const graticule = d3.geoGraticule()
        .step([10, 10])

      this.svg.append('path')
        .datum(graticule)
        .attr('class', 'graticule')
        .attr('d', this.path)
        .style('fill', '#222222')
        .style('stroke', 'white')
    },

    async drawMarkers () {
      const markers = this.markerGroup.selectAll('circle')
        .data(this.positions)

      const center = [this.width / 2 + this.offset, this.height / 2]

      markers
        .enter()
        .append('circle')
        .on('mousedown', (pose) => {
          d3.event.stopPropagation()

          if (d3.event.button === 0) {
            document.getElementById('globe').onmousemove = (event) => {
              const m = document.getElementById('globe').getScreenCTM()
              let p = document.getElementById('globe').createSVGPoint()
              p.x = event.clientX
              p.y = event.clientY
              p = p.matrixTransform(m.inverse())
              const coords = this.projection.invert([p.x, p.y])
              pose.lat = coords[1]
              pose.lon = coords[0]
              this.links = []
              this.updateGlobe()
            }
          } else if (d3.event.button === 2) {
            this.links = []
            this.positions = this.positions.filter(p => p.lat !== pose.lat && p.lon !== pose.lon)
            event.srcElement.remove()
            this.updateGlobe()
          }
        })
        .merge(markers)
        .attr('cx', d => this.projection([d.lon, d.lat])[0])
        .attr('cy', d => this.projection([d.lon, d.lat])[1])
        .attr('fill', d => {
          const coordinate = [d.lon, d.lat]
          const gdistance = d3.geoDistance(coordinate, this.projection.invert(center))
          return gdistance > 1.57 ? 'none' : 'red'
        })
        .attr('r', 7)

      this.markerGroup.each(function () {
        this.parentNode.appendChild(this)
      })
    },

    async drawArcs () {
      if (this.links.length > 0) {
        const geoPath = d3.geoPath(this.projection)
        const poses = {
          type: 'LineString',
          coordinates: []
        }

        this.links.forEach(l => { poses.coordinates.push([l.start.lon, l.start.lat]) })
        poses.coordinates.push([this.links[this.links.length - 1].end.lon, this.links[this.links.length - 1].end.lat])

        this.markerGroup.append('path')
          .attr('d', geoPath(poses))
          .attr('fill', 'transparent')
          .attr('stroke-width', 3)
          .attr('stroke', 'white')
      }
    },

    async updateGlobe () {
      this.markerGroup.selectAll('circle').remove()
      this.markerGroup.selectAll('path').remove()
      this.drawMarkers()
      this.drawArcs()
    },

    async addPosition (pose) {
      this.positions.push(pose)
      this.links = []
      this.updateGlobe()
    },

    async handleValidPos () {
      if (this.address.length > 0) {
        const position = await geoCoder.getCoords(this.address)

        if (position != null) {
          const pose = {
            id: Math.floor(Math.random() * 10000),
            name: this.address,
            lat: position.lat,
            lon: position.lon
          }

          await this.addPosition(pose)
          this.address = ''

          this.invalid = this.positions.Count < 3
        }
      }
    },

    async switchSteps () {
      if (this.updatesOn) {
        this.updateText = 'Turn Updates On'
      } else {
        this.updateText = 'Turn Updates Off'
      }

      this.updatesOn = !this.updatesOn
    },

    async runSimAnnealing () {
      this.running = true
      this.connection.send(JSON.stringify({
        positions: this.positions,
        temperature: parseInt(this.maxTemp),
        decrement: parseFloat(this.decrement),
        threshold: parseFloat(this.threshold),
        updatesOn: this.updatesOn
      }))
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
</style>
