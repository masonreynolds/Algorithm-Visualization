<template>
  <h1 class="text" v-if="states === 0">Minimax Checkers AI</h1>
  <h1 class="text" v-else>Minimax Checkers AI - {{ states }} States Generated</h1>
  <br />

  <div class="row">
    <div class="displayCol">
      <div id="table"></div>
    </div>
    <div class="column">
      <h3 class="text">Number of Future Moves to Generate: {{ depth }}</h3>
      <div class="slidecontainer">
        <input type="range" min="1" max="5" class="slider" id="myRange" v-model="depth">
      </div>
      <br />
      <button type="button" class="btn btn-primary mr-2" @click="runMinimax">Submit Move</button>
      <button type="button" class="btn btn-primary mr-2" @click="resetBoard">Reset Board</button>
      <button type="button" class="btn btn-primary mr-2" @click="undoMove">Undo Move</button>

      <h3 class="text">Moves made: </h3>
      <p id="locations" class="text" style="white-space: pre-wrap" v-if="boards.length === 1">None so far</p>
      <span v-else>
        <p class="text" v-for="board in boards" v-bind:key=board.move>
          <b>{{ board.move }}</b>
        </p>
      </span>
    </div>
  </div>
</template>

<script>
import * as d3 from 'd3'

export default {
  name: 'CheckersAI',
  data () {
    return {
      xAxis: { 0: 'H', 1: 'G', 2: 'G', 3: 'E', 4: 'D', 5: 'C', 6: 'B', 7: 'A' },
      boards: [],
      fieldSize: 90,
      svg: null,
      states: 0,
      depth: 3
    }
  },
  created: async function () {
  },
  mounted: async function () {
    await this.createDefaultBoard()
    await this.drawCheckersBoard()
    await this.updateCheckers()
  },
  methods: {
    async createDefaultBoard () {
      const b = {
        blackPoses: [],
        blackKings: [],
        redPoses: [],
        redKings: [],
        move: 'Start of Game'
      }

      for (let i = 0; i < 3; i++) {
        for (let k = 0; k < 4; k++) {
          b.blackPoses.push([i, (k * 2) + ((i + 1) % 2)])
          b.redPoses.push([7 - i, 7 - ((k * 2) + ((i + 1) % 2))])
        }
      }

      this.boards.push(b)
    },

    async drawCheckersBoard () {
      const boardSize = 8 * this.fieldSize
      const b = []

      for (let i = 0; i < 64; i++) {
        b.push({
          x: i % 8,
          y: Math.floor(i / 8)
        })
      };

      const row = d3.select('#table').append('div')

      this.svg = row.append('div')
        .style('width', boardSize + 'px')
        .style('height', boardSize + 'px')
        .append('svg')
        .attr('width', boardSize + 'px')
        .attr('height', boardSize + 'px')

      this.svg.selectAll('.fields')
        .data(b)
        .enter()
        .append('rect')
        .style('class', 'fields')
        .style('class', 'rects')
        .attr('x', (d) => {
          return d.x * this.fieldSize
        })
        .attr('y', (d) => {
          return d.y * this.fieldSize
        })
        .attr('width', this.fieldSize + 'px')
        .attr('height', this.fieldSize + 'px')
        .style('fill', function (d) {
          if (((d.x % 2 === 0) && (d.y % 2 === 0)) ||
            ((d.x % 2 === 1) && (d.y % 2 === 1))) { return 'red' } else { return 'black' }
        })
        .style('stroke-width', 1)
        .style('stroke', 'black')

      this.svg.on('contextmenu', function () {
        d3.event.preventDefault()
      })
    },

    async updateCheckers () {
      this.svg.selectAll('circle').remove()

      const b = {
        blackPoses: structuredClone(this.boards[this.boards.length - 1].blackPoses),
        blackKings: structuredClone(this.boards[this.boards.length - 1].blackKings),
        redPoses: structuredClone(this.boards[this.boards.length - 1].redPoses),
        redKings: structuredClone(this.boards[this.boards.length - 1].redKings)
      }

      this.updateRedPoses(b)
      this.updateRedKings(b)
      this.updateBlackPoses(b)
      this.updateBlackKings(b)
    },

    async updateRedPoses (b) {
      const reds = this.svg.selectAll('red')
        .data(b.redPoses)
        .enter()
        .append('g')
        .attr('y', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('x', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('width', (this.fieldSize / 2 - 10) + 'px')
        .attr('height', (this.fieldSize / 2 - 10) + 'px')
        .on('mousedown', (pose) => {
          if (d3.event.button === 0) {
            d3.event.target.onmousemove = function (event) {
              const CTM = event.target.getScreenCTM()
              event.target.setAttributeNS(null, 'cx', (event.clientX - CTM.e) / CTM.a)
              event.target.setAttributeNS(null, 'cy', (event.clientY - CTM.f) / CTM.d)
            }

            d3.event.target.onmouseup = (event) => {
              const newPose = [0, 0]
              const CTM = event.target.getScreenCTM()
              newPose[0] = Math.round(((event.clientY - CTM.f) / CTM.d - this.fieldSize / 2) / (this.fieldSize))
              newPose[1] = Math.round(((event.clientX - CTM.e) / CTM.a - this.fieldSize / 2) / (this.fieldSize))
              event.target.setAttributeNS(null, 'cx', newPose[1] * this.fieldSize + (this.fieldSize / 2))
              event.target.setAttributeNS(null, 'cy', newPose[0] * this.fieldSize + (this.fieldSize / 2))

              if ((newPose[0] + newPose[1]) % 2 !== 0) {
                const checkerAtPose = this.boards[this.boards.length - 1].redPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                                      this.boards[this.boards.length - 1].blackPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                                      this.boards[this.boards.length - 1].redKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                                      this.boards[this.boards.length - 1].blackKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1]))

                if (!checkerAtPose) {
                  if (newPose[0] === 0) {
                    b.redPoses = b.redPoses.filter(r => r[0] !== pose[0] || r[1] !== pose[1])
                    b.redKings.push(newPose)

                    event.target.setAttributeNS(null, 'stroke', 'gold')
                  }

                  b.move = `Red: moved from ${this.xAxis[pose[0]]}${pose[1] + 1} to ${this.xAxis[newPose[0]]}${newPose[1] + 1}`
                  pose[0] = newPose[0]
                  pose[1] = newPose[1]
                  this.boards.push(b)
                }
              }

              this.updateCheckers()
              this.onmousemove = null
            }
          } else if (d3.event.button === 2) {
            d3.event.preventDefault()
            b.redPoses = b.redPoses.filter(b => b[0] !== pose[0] || b[1] !== pose[1])
            d3.event.target.remove()

            b.move = `Black: took red at ${this.xAxis[pose[0]]}${pose[1] + 1}`
            this.boards.push(b)
            this.updateCheckers()
          }
        })

      reds.append('circle')
        .attr('cy', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('cx', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('r', (this.fieldSize / 2 - 10) + 'px')
        .attr('fill', 'red')
        .attr('stroke', 'white')
        .attr('stroke-width', 3)
    },

    async updateRedKings (b) {
      const redKings = this.svg.selectAll('redKing')
        .data(b.redKings)
        .enter()
        .append('g')
        .attr('y', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('x', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('width', (this.fieldSize / 2 - 10) + 'px')
        .attr('height', (this.fieldSize / 2 - 10) + 'px')
        .on('mousedown', (pose) => {
          if (d3.event.button === 0) {
            d3.event.target.onmousemove = function (event) {
              const CTM = event.target.getScreenCTM()
              event.target.setAttributeNS(null, 'cx', (event.clientX - CTM.e) / CTM.a)
              event.target.setAttributeNS(null, 'cy', (event.clientY - CTM.f) / CTM.d)
            }

            d3.event.target.onmouseup = (event) => {
              const newPose = [0, 0]
              const CTM = event.target.getScreenCTM()
              newPose[0] = Math.round(((event.clientY - CTM.f) / CTM.d - this.fieldSize / 2) / (this.fieldSize))
              newPose[1] = Math.round(((event.clientX - CTM.e) / CTM.a - this.fieldSize / 2) / (this.fieldSize))
              event.target.setAttributeNS(null, 'cx', newPose[1] * this.fieldSize + (this.fieldSize / 2))
              event.target.setAttributeNS(null, 'cy', newPose[0] * this.fieldSize + (this.fieldSize / 2))

              if ((newPose[0] + newPose[1]) % 2 !== 0) {
                const checkerAtPose = this.boards[this.boards.length - 1].redPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                        this.boards[this.boards.length - 1].blackPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                        this.boards[this.boards.length - 1].redKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                        this.boards[this.boards.length - 1].blackKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1]))

                if (!checkerAtPose) {
                  b.move = `Red: moved from ${this.xAxis[pose[0]]}${pose[1] + 1} to ${this.xAxis[newPose[0]]}${newPose[1] + 1}`
                  pose[0] = newPose[0]
                  pose[1] = newPose[1]
                  this.boards.push(b)
                }
              }

              this.updateCheckers()
              this.onmousemove = null
            }
          } else if (d3.event.button === 2) {
            d3.event.preventDefault()
            b.redKings = b.redKings.filter(b => b[0] !== pose[0] || b[1] !== pose[1])
            d3.event.target.remove()

            b.move = `Red: took black at ${this.xAxis[pose[0]]}${pose[1] + 1}`
            this.boards.push(b)
            this.updateCheckers()
          }
        })

      redKings.append('circle')
        .attr('cy', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('cx', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('r', (this.fieldSize / 2 - 10) + 'px')
        .attr('fill', 'red')
        .attr('stroke', 'gold')
        .attr('stroke-width', 3)
    },

    async updateBlackPoses (b) {
      const blacks = this.svg.selectAll('black')
        .data(b.blackPoses)
        .enter()
        .append('g')
        .attr('y', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('x', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('width', (this.fieldSize / 2 - 10) + 'px')
        .attr('height', (this.fieldSize / 2 - 10) + 'px')
        .on('mousedown', (pose) => {
          if (d3.event.button === 0) {
            d3.event.target.onmousemove = function (event) {
              const CTM = event.target.getScreenCTM()
              event.target.setAttributeNS(null, 'cx', (event.clientX - CTM.e) / CTM.a)
              event.target.setAttributeNS(null, 'cy', (event.clientY - CTM.f) / CTM.d)
            }

            d3.event.target.onmouseup = (event) => {
              const newPose = [0, 0]
              const CTM = event.target.getScreenCTM()
              newPose[0] = Math.round(((event.clientY - CTM.f) / CTM.d - this.fieldSize / 2) / (this.fieldSize))
              newPose[1] = Math.round(((event.clientX - CTM.e) / CTM.a - this.fieldSize / 2) / (this.fieldSize))
              event.target.setAttributeNS(null, 'cx', newPose[1] * this.fieldSize + (this.fieldSize / 2))
              event.target.setAttributeNS(null, 'cy', newPose[0] * this.fieldSize + (this.fieldSize / 2))

              if ((newPose[0] + newPose[1]) % 2 !== 0) {
                const checkerAtPose = this.boards[this.boards.length - 1].redPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                                      this.boards[this.boards.length - 1].blackPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                                      this.boards[this.boards.length - 1].redKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                                      this.boards[this.boards.length - 1].blackKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1]))

                if (!checkerAtPose) {
                  if (newPose[0] === 7) {
                    b.blackPoses = b.blackPoses.filter(r => r[0] !== pose[0] || r[1] !== pose[1])
                    b.blackKings.push(newPose)

                    event.target.setAttributeNS(null, 'stroke', 'gold')
                  }

                  b.move = `Black: moved from ${this.xAxis[pose[0]]}${pose[1] + 1} to ${this.xAxis[newPose[0]]}${newPose[1] + 1}`
                  pose[0] = newPose[0]
                  pose[1] = newPose[1]
                  this.boards.push(b)
                }
              }

              this.updateCheckers()
              this.onmousemove = null
            }
          } else if (d3.event.button === 2) {
            d3.event.preventDefault()
            b.blackPoses = b.blackPoses.filter(b => b[0] !== pose[0] || b[1] !== pose[1])
            d3.event.target.remove()

            b.move = `Black: took red at ${this.xAxis[pose[0]]}${pose[1] + 1}`
            this.boards.push(b)
            this.updateCheckers()
          }
        })

      blacks.append('circle')
        .attr('cy', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('cx', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('r', (this.fieldSize / 2 - 10) + 'px')
        .attr('fill', 'black')
        .attr('stroke', 'white')
        .attr('stroke-width', 3)
    },

    async updateBlackKings (b) {
      const blackKings = this.svg.selectAll('blackKing')
        .data(b.blackKings)
        .enter()
        .append('g')
        .attr('y', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('x', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('width', (this.fieldSize / 2 - 10) + 'px')
        .attr('height', (this.fieldSize / 2 - 10) + 'px')
        .on('mousedown', (pose) => {
          if (d3.event.button === 0) {
            d3.event.target.onmousemove = function (event) {
              const CTM = event.target.getScreenCTM()
              event.target.setAttributeNS(null, 'cx', (event.clientX - CTM.e) / CTM.a)
              event.target.setAttributeNS(null, 'cy', (event.clientY - CTM.f) / CTM.d)
            }

            d3.event.target.onmouseup = (event) => {
              const newPose = [0, 0]
              const CTM = event.target.getScreenCTM()
              newPose[0] = Math.round(((event.clientY - CTM.f) / CTM.d - this.fieldSize / 2) / (this.fieldSize))
              newPose[1] = Math.round(((event.clientX - CTM.e) / CTM.a - this.fieldSize / 2) / (this.fieldSize))
              event.target.setAttributeNS(null, 'cx', newPose[1] * this.fieldSize + (this.fieldSize / 2))
              event.target.setAttributeNS(null, 'cy', newPose[0] * this.fieldSize + (this.fieldSize / 2))

              if ((newPose[0] + newPose[1]) % 2 !== 0) {
                const checkerAtPose = this.boards[this.boards.length - 1].redPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                        this.boards[this.boards.length - 1].blackPoses.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                        this.boards[this.boards.length - 1].redKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1])) ||
                        this.boards[this.boards.length - 1].blackKings.some((r) => (r[0] === newPose[0] && r[1] === newPose[1]))

                if (!checkerAtPose) {
                  b.move = `Black: moved from ${this.xAxis[pose[0]]}${pose[1] + 1} to ${this.xAxis[newPose[0]]}${newPose[1] + 1}`
                  pose[0] = newPose[0]
                  pose[1] = newPose[1]
                  this.boards.push(b)
                }
              }

              this.updateCheckers()
              this.onmousemove = null
            }
          } else if (d3.event.button === 2) {
            d3.event.preventDefault()
            b.blackKings = b.blackKings.filter(b => b[0] !== pose[0] || b[1] !== pose[1])
            d3.event.target.remove()

            b.move = `Red: took black at ${this.xAxis[pose[0]]}${pose[1] + 1}`
            this.boards.push(b)
            this.updateCheckers()
          }
        })

      blackKings.append('circle')
        .attr('cy', d => (((d[0] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('cx', d => (((d[1] + 1) * this.fieldSize) - (this.fieldSize / 2)) + 'px')
        .attr('r', (this.fieldSize / 2 - 10) + 'px')
        .attr('fill', 'black')
        .attr('stroke', 'gold')
        .attr('stroke-width', 3)
    },

    async undoMove () {
      if (this.boards.length > 1) {
        this.boards.pop()
        this.states = 0
        this.updateCheckers()
      }
    },

    async resetBoard () {
      this.boards = []
      this.createDefaultBoard()
      this.updateCheckers()
      this.states = 0
    },

    runMinimax: async () => {
      // const result = Minimax.startMinimax(boards[this.boards.length - 1], depth)
      // this.boards.Add(result.board)
      // this.states = result.states
      // this.StateHasChanged()

      // await JSRuntime.InvokeVoidAsync('updateCheckers', boards[this.boards.length - 1])
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>

</style>
