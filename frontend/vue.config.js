const { defineConfig } = require('@vue/cli-service')
module.exports = defineConfig({
  transpileDependencies: true,
  devServer: {
    client: {
      progress: true
    },
    hot: true,
    watchFiles: {
      paths: ['src/**/*.js', 'src/**/*.vue', 'src/**/*.json', 'src/views/*.vue']
    },
    liveReload: true,
  }
})
