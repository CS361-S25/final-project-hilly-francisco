require(ggplot2)
require(scales)

#These are attractive and color-blind friendly colors to use
#Could also use the viridis package: https://cran.r-project.org/web/packages/viridis/vignettes/intro-to-viridis.html
colors <- c("#673F03", "#B50142","#AB08FF","#7B1DFF", "#5731FD","#5E8EFF", "#4755FF" ,"#6FC4FE", "#86E9FE", "#96FFF7", "#B2FCE3", "#BBFFDB", "#D4FFDD", "#EFFDF0")

#Set your working directory
# TODO: Change PATH_TO_YOUR_FOLDER
setwd("PATH_TO_YOUR_FOLDER/Example/")

#You can have whatever variables you want like this
pop_cap <- 10000

#Read in your data
initial_data <- read.table("munged_basic.dat", h=T)

#You can add extra columns with calculations with cbind
initial_data <-cbind(initial_data, hist_0_prop=initial_data$hist_0/initial_data$count, Treatment=as.factor(initial_data$treatment))

#You can get just a portion of your data with subset
early <- subset(initial_data, update<20)
just_middle <- subset(initial_data, treatment==0.5)

#If you have NaN values that you would like to be 0, this is how
zeroed <- initial_data
zeroed[is.na(zeroed)] <- 0


#This will make a line plot over time
#It's not very exciting currently...
ggplot(data=early, aes(x=update, y=coop, group=Treatment, colour=Treatment)) + ylab("Mean Cooperation Value") + xlab("Updates") + stat_summary(aes(color=Treatment, fill=Treatment),fun.data="mean_cl_boot", geom=c("smooth"), se=TRUE) + theme(panel.background = element_rect(fill='white', colour='black')) + theme(panel.grid.major = element_blank(), panel.grid.minor = element_blank()) + guides(fill=FALSE) +scale_colour_manual(values=colors) + scale_fill_manual(values=colors) +xlim(0,20)

update_1 <- subset(initial_data, update==1)

# This will make a point plot
ggplot(data=update_1, aes(x=count, y=coop, group=Treatment, colour=Treatment)) +ylab("Mean Cooperation Value") + xlab("Number of Organisms") + geom_point() + scale_colour_manual(values=colors)

#Box plot
ggplot(data=update_1, aes(x=Treatment, y=coop)) + geom_boxplot() 

# Non-parametric test to see if two sets of data are probably different
wilcox.test(subset(update_1, treatment==1.0)$coop, subset(update_1, treatment=0.5)$coop)
