<script setup>
import { ref, computed } from 'vue'
import Home from './components/Home.vue'
import About from './components/About.vue'
import Create from './components/Create.vue'
import Validate from './components/Validate.vue'
import NotFound from './components/NotFound.vue'

const routes = {
  '/': Home,
  '/about': About,
  '/create': Create,
  '/validate': Validate,
}

const currentPath = ref(window.location.hash)

window.addEventListener('hashchange', () => {
  currentPath.value = window.location.hash
})

const currentView = computed(() => {
  return routes[currentPath.value.slice(1) || '/'] || NotFound
})
</script>

<template>
  <component :is="currentView" /><br /><br />
  <a href="#/">Home</a> 
  <a href="#/about">About</a> 
  <a href="#/create">Create</a> 
  <a href="#/validate">Validate</a> 
  <a href="#/non-existent-path">Broken Link</a> 
</template>
