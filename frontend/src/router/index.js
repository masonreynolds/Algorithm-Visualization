import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '../views/HomeView.vue'
import TravelingSalesmanView from '../views/TravelingSalesmanView.vue'
import CheckersView from '../views/CheckersView.vue'

const routes = [
  {
    path: '/',
    name: 'home',
    component: HomeView
  },
  {
    path: '/TravelingSalesman',
    name: 'TravelingSalesman',
    component: TravelingSalesmanView
  },
  {
    path: '/Checkers',
    name: 'Checkers',
    component: CheckersView
  }
]

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes
})

export default router
