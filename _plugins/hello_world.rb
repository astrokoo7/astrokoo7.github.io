# hello_world.rb

module Jekyll
    module HelloWorld
      def hello_world
        "Hello World"
      end
    end
  end
  
  Liquid::Template.register_filter(Jekyll::HelloWorld)