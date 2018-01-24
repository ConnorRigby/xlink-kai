use Mix.Config

config :xlink, dl_url:
  [
    arm: "http://cdn.teamxlink.co.uk/binary/kaiEngine-7.4.30-rev595.headless.ARM.tar.gz",
    x86: "http://cdn.teamxlink.co.uk/binary/kaiEngine-7.4.30-rev595.headles.el5.i386.tar.gz"
  ]
config :xlink, filename: "kaiEngine-7.4.30-rev595.headless.ARM.tar.gz"
config :xlink, data_dir: "tmp/xlink_data"
config :xlink, config_file: "/boot/xlink-config.txt"
# config :xlink, config_file: "xlink-config.txt"
import_config "#{Mix.Project.config[:target]}.exs"
