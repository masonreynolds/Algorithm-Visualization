<template>
    <PathFinder/>
  </template>

<script>
import PathFinder from '@/components/PathFinder.vue'

export default {
  name: 'App',
  components: {
    PathFinder
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
