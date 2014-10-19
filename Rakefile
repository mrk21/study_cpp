require 'fileutils'

directory 'gen'

task :default => :build

desc 'Build'
task :build => 'gen' do
  target = ENV['t']
  Dir.chdir('gen') do
    sh 'cmake ..'
    if target.nil? then
      sh 'make'
    else
      sh "make #{target} && ./#{target}"
    end
  end
end

desc 'Clean'
task :clean => 'gen' do
  Dir.chdir('gen') do
    sh 'make clean'
  end
end

desc 'Rebuild'
task :rebuild => 'gen' do
  Dir.chdir('gen') do
    sh 'make clean all'
  end
end

desc 'Distclean'
task :distclean do
  FileUtils.rm_rf 'gen'
end
