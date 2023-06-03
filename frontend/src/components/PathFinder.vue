<template>
  <h1 class='text'>Q-Learning PathFinder AI</h1>
  <br />

  <div class='row'>
    <div class='displayCol'>
      <div id='table'></div>
    </div>
    <div class='column'>
      <h3 class='text'>Size of the Environment: {{ dimensions }}</h3>
      <div class='slidecontainer'>
        <input type='range' min='5' max='20' class='slider' id='myRange' v-model='dimensions' v-on:change='resetEnvironment()' :lazy='true'>
      </div>
      <br />
      <button type='button' class='btn btn-primary mr-2' @click='runPolicyLearning'>Start Exploring</button>
      <button type='button' class='btn btn-primary mr-2' @click='resetEnvironment'>Reset Board</button>
    </div>
  </div>
</template>

<script>
import * as d3 from 'd3'

export default {
  name: 'PathFinder',
  data () {
    return {
      connection: null,
      environment: null,
      environmentSize: 720,
      iterations: 0,
      dimensions: 5,
      running: false,
      svg: null,
      source: null,
      loc: null
    }
  },
  computed: {
    cellSize () {
      return this.environmentSize / this.dimensions
    },
    radius () {
      return (this.cellSize / 2 - 10)
    }
  },
  created: function () {
    this.connection = new WebSocket('ws://localhost:18080/Path-Finder-WS')

    this.connection.onmessage = async (event) => {
      const data = JSON.parse(event.data)
      this.environment.agent = data.environment.agent
      this.environment.rewards = data.environment.rewards
      await this.drawEnvironment()
      await this.updateEnvironment()
    }
  },
  mounted: async function () {
    await this.createDefaultEnvironment()
    await this.drawEnvironment()
    await this.updateEnvironment()
  },
  methods: {
    getDragHandler () {
      return d3.drag()
        .on('start', (event) => {
          this.source = d3.select(event.sourceEvent.target)
          this.loc = [
            Math.round((event.x - this.cellSize / 2) / (this.cellSize)),
            Math.round((event.y - this.cellSize / 2) / (this.cellSize))
          ]
        })
        .on('drag', function (event) {
          d3.select(this)
            .attr('cx', event.x)
            .attr('cy', event.y)
        })
        .on('end', (event) => {
          const target = d3.select(event.sourceEvent.target)
          const newPose = [
            Math.round((event.x - this.cellSize / 2) / (this.cellSize)),
            Math.round((event.y - this.cellSize / 2) / (this.cellSize))
          ]
          const blockAtPose = this.environment.blocks.some((b) => (b[0] === newPose[0] && b[1] === newPose[1]))
          const thingAtPose = (newPose[0] === this.environment.agent[0] && newPose[1] === this.environment.agent[1]) ||
              (newPose[0] === this.environment.goal[0] && newPose[1] === this.environment.goal[1])

          if (!blockAtPose && !thingAtPose) {
            const targetClass = target.attr('class')
            if (targetClass === 'agent') {
              this.environment.agent = newPose
            } else if (targetClass === 'goal') {
              this.environment.goal = newPose
            }

            target
              .attr('cx', (newPose[0] * this.cellSize) + (this.cellSize / 2))
              .attr('cy', (newPose[1] * this.cellSize) + (this.cellSize / 2))
          } else {
            this.source
              .attr('cx', (this.loc[0] * this.cellSize) + (this.cellSize / 2))
              .attr('cy', (this.loc[1] * this.cellSize) + (this.cellSize / 2))

            this.loc = null
            this.source = null
          }
        })
    },

    async createDefaultEnvironment () {
      this.environment = {
        agent: [0, 0],
        goal: [this.dimensions - 1, this.dimensions - 1],
        blocks: [],
        rewards: []
      }

      const dims = parseInt(this.dimensions)
      for (let i = 0; i < dims; i++) {
        this.environment.rewards.push(new Array(dims).fill(0))
      }
    },

    async drawEnvironment () {
      if (this.svg) {
        d3.select('#table').select('svg').remove()
      }

      const e = []
      for (let i = 0; i < this.dimensions ** 2; i++) {
        e.push({
          x: i % this.dimensions,
          y: Math.floor(i / this.dimensions),
          r: this.environment.rewards[i % this.dimensions][Math.floor(i / this.dimensions)]
        })
      };

      this.svg = d3.select('#table')
        .style('width', this.environmentSize + 'px')
        .style('height', this.environmentSize + 'px')
        .append('svg')
        .attr('width', this.environmentSize + 'px')
        .attr('height', this.environmentSize + 'px')

      this.svg.selectAll('.cells')
        .data(e)
        .enter()
        .append('rect')
        .attr('class', 'cells')
        .attr('x', (d) => {
          return d.x * this.cellSize
        })
        .attr('y', (d) => {
          return d.y * this.cellSize
        })
        .attr('width', this.cellSize + 'px')
        .attr('height', this.cellSize + 'px')
        .attr('fill', (d) => {
          if (d.r >= 0) {
            const hexString = (127 + Math.round(d.r * 128)).toString(16)
            return '#80' + hexString + '80'
          } else {
            const hexString = (127 - Math.round(d.r * 128)).toString(16)
            return '#' + hexString + '8080'
          }
        })
        .attr('stroke-width', 1)
        .attr('stroke', 'black')
        .on('click', (event) => {
          const target = d3.select(event.target)
          if (event.button === 0) {
            const newPose = [
              Math.round((target.attr('x') - this.cellSize / 2) / (this.cellSize)),
              Math.round((target.attr('y') - this.cellSize / 2) / (this.cellSize))
            ]

            const thingAtPose = (newPose[0] === this.environment.agent[0] && newPose[1] === this.environment.agent[1]) ||
              (newPose[0] === this.environment.goal[0] && newPose[1] === this.environment.goal[1])

            if (!thingAtPose) {
              target.attr('fill', 'gray')
              this.environment.rewards[newPose[0]][newPose[1]] = 0
              this.environment.blocks.push(newPose)
              this.createBlock(target.attr('x'), target.attr('y'))
            }
          }
        })

      this.svg.on('contextmenu', function () {
        event.preventDefault()
      })
    },

    async updateEnvironment () {
      this.updateAgent()
      this.updateGoal()
    },

    async updateAgent () {
      const agent = this.svg.append('circle')
        .attr('class', 'agent')
        .attr('cy', (this.environment.agent[0] * this.cellSize) + (this.cellSize / 2) + 'px')
        .attr('cx', (this.environment.agent[1] * this.cellSize) + (this.cellSize / 2) + 'px')
        .attr('r', this.radius + 'px')
        .attr('fill', 'red')
        .attr('stroke', 'black')
        .attr('stroke-width', 3)

      this.getDragHandler()(agent)
    },

    async updateGoal () {
      const goal = this.svg.append('circle')
        .attr('class', 'goal')
        .attr('cy', (this.environment.goal[0] * this.cellSize) + (this.cellSize / 2) + 'px')
        .attr('cx', (this.environment.goal[1] * this.cellSize) + (this.cellSize / 2) + 'px')
        .attr('r', this.radius + 'px')
        .attr('fill', 'green')
        .attr('stroke', 'black')
        .attr('stroke-width', 3)

      this.getDragHandler()(goal)
    },

    async createBlock (x, y) {
      this.svg.append('rect')
        .attr('class', 'block')
        .attr('x', x + 'px')
        .attr('y', y + 'px')
        .attr('width', this.cellSize + 'px')
        .attr('height', this.cellSize + 'px')
        .attr('fill', 'black')
        .on('mousedown', (event) => {
          if (event.button === 2) {
            const pose = [
              Math.round((event.target.x.baseVal.value - this.cellSize / 2) / (this.cellSize)),
              Math.round((event.target.y.baseVal.value - this.cellSize / 2) / (this.cellSize))
            ]
            this.environment.blocks = this.environment.blocks.filter(b => b[0] !== pose[0] || b[1] !== pose[1])
            event.target.remove()
          }
        })
    },

    async resetEnvironment () {
      await this.createDefaultEnvironment()
      await this.drawEnvironment()
      await this.updateEnvironment()
    },

    async runPolicyLearning () {
      console.log(++this.iterations)
      this.running = true
      this.connection.send(JSON.stringify({
        environment: this.environment
      }))
    }
  }
}
</script>

<!-- Add 'scoped' attribute to limit CSS to this component only -->
<style scoped>

</style>
