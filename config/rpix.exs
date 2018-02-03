use Mix.Config
config :nerves, :firmware,
  rootfs_overlay: "config/rootfs_additions_rpix",
  fwup_conf: "fwup-#{Mix.Project.config()[:target]}.conf"

config :nerves_firmware_ssh, authorized_keys: [File.read!(Path.join(System.user_home!(), ".ssh/id_rsa.pub"))]

config :bootloader,
  init: [:nerves_runtime, :nerves_network, :nerves_firmware_ssh],
  app: Mix.Project.config()[:app]


config :xlink, data_dir: "/root/xlink_data"
