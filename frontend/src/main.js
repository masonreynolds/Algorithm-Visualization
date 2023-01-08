import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import './assets/css/main.css'

import '@fortawesome/fontawesome-free/js/all'

createApp(App).use(router).mount('#app')
