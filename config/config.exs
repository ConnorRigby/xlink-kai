use Mix.Config

config :xlink,
  dl_url: [
    arm: "http://cdn.teamxlink.co.uk/binary/kaiEngine-7.4.30-rev595.headless.ARM.tar.gz",
    x86: "http://cdn.teamxlink.co.uk/binary/kaiEngine-7.4.30-rev595.headles.el5.i386.tar.gz"
  ]

config :xlink, filename: "kaiEngine-7.4.30-rev595.headless.ARM.tar.gz"
config :xlink, data_dir: "tmp/xlink_data"
config :xlink, config_file: "/boot/xlink-config.txt"

config :logger,
  backends: [
    :console,
    {ExSyslogger, :ex_syslogger_info},
    # {ExSyslogger, :ex_syslogger_debug},
    # {ExSyslogger, :ex_syslogger_warn},
    # {ExSyslogger, :ex_syslogger_info},
  ]
config :logger, :ex_syslogger_info,
  level: :info,
  format: "$date $time [$level] $message",
  ident: "XLINK",
  facility: :local1,
  option: [:pid, :perror]

config :logger, :ex_syslogger_debug,
  level: :debug,
  format: "$date $time [$level] $message",
  ident: "XLINK",
  facility: :local1,
  option: [:pid, :perror]

config :logger, :ex_syslogger_warn,
  level: :warn,
  format: "$date $time [$level] $message",
  ident: "XLINK",
  facility: :local1,
  option: [:pid, :perror]

config :logger, :ex_syslogger_error,
  level: :error,
  format: "$date $time [$level] $message",
  ident: "XLINK",
  facility: :local1,
  option: [:pid, :perror]

target = Mix.Project.config()[:target]

if String.contains?(target, "rpi") do
  import_config "rpix.exs"
else
  import_config "#{target}.exs"
end
