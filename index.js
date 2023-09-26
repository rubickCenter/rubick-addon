let rubickAddon;

if (process.platform === 'darwin') {
    rubickAddon = require("./permissionCheck");
} else {
    rubickAddon = require("bindings")("rubick-addon");
}

module.exports = rubickAddon;
