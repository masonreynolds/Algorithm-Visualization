import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '../views/HomeView.vue'
import TravelingSalesmanView from '../views/TravelingSalesmanView.vue'
import CheckersView from '../views/CheckersView.vue'
import PathFinderView from '../views/PathFinderView.vue'

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
  },
  {
    path: '/PathFinder',
    name: 'PathFinder',
    component: PathFinderView
  }
]

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes
})

export default router
