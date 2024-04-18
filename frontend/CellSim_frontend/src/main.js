import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import MathJaxVuewer from "@dongivan/mathjax-vuewer"
import router from './router'

const app = createApp(App)

app.use(router)
app.use(MathJaxVuewer, {
    loader: {
        load: ["input/tex-base", "input/mml", "output/chtml", "output/svg", "[mml]/html"],
    },
    tex: {
        packages: {
        "[+]": ["base"],
        },
    },
    startup: {
        output: ["chtml", "svg"],
    },
})

app.mount('#app')
