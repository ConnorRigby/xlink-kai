use Mix.Config

config :xlink,
  dl_url: [
    arm: "http://cdn.teamxlink.co.uk/binary/kaiEngine-7.4.30-rev595.headless.ARM.tar.gz",
    x86: "http://cdn.teamxlink.co.uk/binary/kaiEngine-7.4.30-rev595.headles.el5.i386.tar.gz"
  ]

config :xlink, filename: "kaiEngine-7.4.33-rev759.headless.ARM.tar.gz"
config :xlink, data_dir: "tmp/xlink_data"
config :xlink, config_file: "/boot/xlink-config.json"

config :logger,
  backends: [RingLogger, :console],
  handle_otp_reports: true,
  handle_sasl_reports: true

target = Mix.Project.config()[:target]

if String.contains?(target, "rpi") do
  import_config "rpix.exs"
else
  import_config "#{target}.exs"
end
