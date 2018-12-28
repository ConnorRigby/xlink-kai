use Mix.Config

config :nerves, :firmware,
  rootfs_overlay: "config/rootfs_additions_rpix",
  fwup_conf: "fwup-#{Mix.Project.config()[:target]}.conf"

config :nerves_firmware_ssh,
  authorized_keys: [
    File.read!(Path.join(System.user_home!, ".ssh/id_rsa.pub"))
  ]

config :shoehorn,
  init: [:nerves_runtime, :nerves_init_gadget],
  app: Mix.Project.config()[:app]

config :xlink, data_dir: "/root/xlink_data"

if Mix.Project.config()[:target] == "rpi0" do
  config :nerves_init_gadget,
  ifname: "usb0",
  address_method: :dhcpd,
  mdns_domain: "xlink.local",
  node_name: "xlink",
  node_host: :mdns_domain
else
  config :nerves_init_gadget,
  ifname: "usb0",
  address_method: :dhcp,
  mdns_domain: "xlink.local",
  node_name: "xlink",
  node_host: :mdns_domain 
end
