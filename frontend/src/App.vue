<template>
  <TravelingSalesman/>
</template>

<script>
import TravelingSalesman from './components/TravelingSalesman.vue'

export default {
  name: 'App',
  components: {
    TravelingSalesman
  }
}
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  background-color: #222222;
  text-align: center;
  color: white;
  margin-top: 60px;
}

body, html {
  background-color: #222222;
  padding: 0;
  margin: 0;
  width: 100%;
  min-height: 100%;
}
</style>
